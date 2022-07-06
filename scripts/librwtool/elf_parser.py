from elftools.elf.enums import ENUM_RELOC_TYPE_ARM
from elftools.elf.relocation import RelocationSection

from librw.symbol import Symbol # need symbol


def export_cmse_fn(elf):
    symtab = elf.get_section_by_name(".symtab")
    if not symtab:
        raise Exception("No symbol table found")

    cmse_fn = dict()
    for s in symtab.iter_symbols():
        if s['st_info']['type'] == "STT_FUNC":
            cmse_fn[s.name] = s['st_value']

    return cmse_fn


def export_data_section(symtab, base, code):
    data_base = base
    data_range = range(data_base, data_base + len(code))
    raw_symbols = list(filter(lambda x: x['st_value'] in data_range and x['st_info']['type'] == 'STT_OBJECT',
                              [s for s in symtab.iter_symbols()]))
    raw_symbols.sort(key=lambda x: x['st_value'])

    prev_symbol = None
    gap_cnt = 0
    data_symbols = list()

    for rs in raw_symbols:
        st_value = rs['st_value']
        st_size = rs['st_size']
        if prev_symbol:
            prev_st_value = prev_symbol['st_value']
            prev_st_size = prev_symbol['st_size']
            if st_value - prev_st_value != prev_st_size:
                gap_size = st_value - prev_st_value - prev_st_size
                gap_addr = prev_st_value + prev_st_size
                start = gap_addr - data_base
                end = start + gap_size
                sym = Symbol(gap_addr, gap_size, code[start:end], "data.gap%d" % gap_cnt)
                setattr(sym, "type", "STT_OBJECT")
                setattr(sym, "anonymous", True)
                data_symbols.append(sym)
                gap_cnt += 1

        start = st_value - data_base
        end = start + st_size
        sym = Symbol(st_value, st_size, code[start:end], rs.name)
        setattr(sym, "type", "STT_OBJECT")
        data_symbols.append(sym)
        prev_symbol = rs

        if rs is raw_symbols[-1] and len(code[end:]) > 0:
            st_value = end + data_base
            st_size = len(code[end:])
            sym = Symbol(st_value, st_size, code[end:], "data.gap%d" % gap_cnt)
            setattr(sym, "type", "STT_OBJECT")
            setattr(sym, "anonymous", True)
            data_symbols.append(sym)

    return data_symbols


def export_text_section(symtab, base, code):
    text_base = base
    text_range = range(base, text_base + len(code))
    addresses = set()
    raw_symbols = list()

    for s in symtab.iter_symbols():
        if s['st_value'] in text_range and s['st_value'] not in addresses:
            if s['st_info']['type'] in ('STT_FUNC', 'STT_OBJECT'):
                if s.name in ('__bhs_ldivmod1', '__aeabi_memcpy4'):
                    continue
                addresses.add(s['st_value'])
                raw_symbols.append(s)
            elif s['st_info']['type'] == 'STT_NOTYPE' and s['st_info']['bind'] == 'STB_GLOBAL' and \
                    not (s.name[-6:] == '_start' or s.name[-4:] == '_end'):
                raw_symbols.append(s)
            else:
                pass

    raw_symbols.sort(key=lambda x: x['st_value'])
    text_symbols = list()
    prev_symbol = None
    gap_cnt = 0
    for i in range(len(raw_symbols)):
        s = raw_symbols[i]
        st_value = s['st_value'] & ~1 if s['st_info']['type'] == 'STT_FUNC' else s['st_value']
        st_size = s['st_size']

        if prev_symbol and s['st_info']['type'] in ('STT_OBJECT', 'STT_FUNC') and \
                prev_symbol['st_info']['type'] in ('STT_OBJECT', 'STT_FUNC'):
            prev_st_value = prev_symbol['st_value'] & ~1 \
                if prev_symbol['st_info']['type'] == 'STT_FUNC' else prev_symbol['st_value']
            prev_st_size = prev_symbol['st_size']
            if st_value - prev_st_value != prev_st_size:
                gap_size = st_value - prev_st_value - prev_st_size
                gap_addr = prev_st_value + prev_st_size
                if gap_addr in text_range:
                    start = gap_addr - text_base
                    end = start + gap_size
                    sym = Symbol(gap_addr, gap_size, code[start:end], "text.gap%d" % gap_cnt)
                    setattr(sym, "type", "STT_OBJECT")
                    setattr(sym, "anonymous", True)
                    text_symbols.append(sym)
                    gap_cnt += 1

        if s.name != '__aeabi_memcpy':
            if st_size == 0 and i < len(raw_symbols) - 1:
                st_size = raw_symbols[i + 1]['st_value'] - st_value
                if raw_symbols[i + 1]['st_info']['type'] == 'STT_FUNC':
                    st_size -= 1
        else:
            st_size = 52

        start = st_value - text_base
        if s['st_info']['type'] == 'STT_FUNC':
            st_value += 1
        end = start + st_size
        fn_code = code[start:end]

        if s['st_info']['type'] == 'STT_NOTYPE':
            st_type = 'STT_OBJECT'
        else:
            st_type = s['st_info']['type']

        sym = Symbol(st_value, st_size, fn_code, s.name)
        setattr(sym, "type", st_type)
        text_symbols.append(sym)
        prev_symbol = s

        if i == len(raw_symbols) - 1:
            if len(code[end:]) > 0:
                st_value = end + text_base
                st_size = len(code[end:])
                fn_code = code[end:]
                sym = Symbol(st_value, st_size, fn_code, "text.gap%d" % gap_cnt)
                setattr(sym, "type", st_type)
                setattr(sym, "anonymous", True)
                text_symbols.append(sym)

    text_symbols.sort(key=lambda x: x.address)

    return text_symbols


def export_pointers(elf, name, text_range):
    reloc_table = elf.get_section_by_name(name)
    if not isinstance(reloc_table, RelocationSection):
        return None

    ptrs = dict()
    symbols = elf.get_section(reloc_table["sh_link"])

    for i in reloc_table.iter_relocations():
        symbol = symbols.get_symbol(i["r_info_sym"])
        r_type = i["r_info_type"]
        r_offset = i["r_offset"]
        if r_type == ENUM_RELOC_TYPE_ARM["R_ARM_ABS32"] and symbol["st_value"] in text_range:
            ptrs[r_offset] = dict(offset=symbol["st_value"], type=symbol["st_info"]["type"])
            if symbol["st_info"]["type"] == 'STT_FUNC':
                ptrs[r_offset]["offset"] -= 1

    return ptrs


def export_symbols(elf):
    symtab = elf.get_section_by_name(".symtab")
    if not symtab:
        raise Exception("No symbol table found")

    text = elf.get_section_by_name(".text")
    if text:
        text_code = text.data()
        text_base = text['sh_addr']
        text_end = text_base + len(text_code)
        text_range = range(text_base, text_end)
        text_symbols = export_text_section(symtab, text_base, text_code)

        data = elf.get_section_by_name(".data")
        data_symbols = None
        if data:
            data_code = data.data()
            data_base = data['sh_addr']
            data_symbols = export_data_section(symtab, data_base, data_code)

        ptrs = export_pointers(elf, ".rel.text", text_range)
        ptrs2 = export_pointers(elf, ".rel.data", text_range)

        if ptrs:
            if ptrs2:
                ptrs.update(ptrs2)
        else:
            ptrs = ptrs2

        if ptrs:
            setattr(Symbol, "reloc", ptrs)

        return text_symbols, text_end, data_symbols
    else:
        raise Exception("No text section!")


def export_fn_symbols(elf):
    symtab = elf.get_section_by_name(".symtab")
    if not symtab:
        raise Exception("No symbol table found")

    strtab = elf.get_section_by_name(".strtab")
    if not strtab:
        raise Exception("No string table found")

    text = elf.get_section_by_name(".text")
    if not text:
        raise Exception("No text section found")

    data_section = elf.get_section_by_name(".data")
    if data_section:
        data = data_section.data()
        data_base = data_section['sh_addr']
        data_range = range(data_base, data_base + len(data))

    code = text.data()
    text_base = text['sh_addr']
    text_range = range(text_base, text_base + len(code))

    fn_symbols = list()
    addresses = set()

    setattr(Symbol, "reloc", export_pointers(elf, text_range, data_range if data_section else None))
    raw_symbols = list()

    etext = text_base + len(code)

    # Extract all functions
    for s in symtab.iter_symbols():
        if s['st_value'] in text_range and s['st_value'] not in addresses:
            if s['st_info']['type'] in ('STT_FUNC', 'STT_OBJECT'):
                if strtab.get_string(s['st_name']) in ('__bhs_ldivmod1', '__aeabi_memcpy4'):
                    continue
                addresses.add(s['st_value'])
                raw_symbols.append(s)
            elif s['st_info']['type'] == 'STT_NOTYPE' and s['st_info']['bind'] == 'STB_GLOBAL' and \
                    not (s.name[-6:] == '_start' or s.name[-4:] == '_end'):
                raw_symbols.append(s)
            else:
                pass
        elif data_section and s['st_value'] in data_range and s['st_info']['type'] == 'STT_OBJECT':
            raw_symbols.append(s)
        else:
            pass

    raw_symbols.sort(key=lambda x: x['st_value'])

    prev_symbol = None
    gap_cnt = 0
    for i in range(len(raw_symbols)):
        s = raw_symbols[i]
        st_value = s['st_value'] & ~1 if s['st_info']['type'] == 'STT_FUNC' else s['st_value']
        st_size = s['st_size']

        if prev_symbol and s['st_info']['type'] in ('STT_OBJECT', 'STT_FUNC') and \
                prev_symbol['st_info']['type'] in ('STT_OBJECT', 'STT_FUNC') and \
                (s['st_value'] in text_range and prev_symbol['st_value'] in text_range or s['st_value'] in data_range \
                 and prev_symbol['st_value'] in data_range):
            prev_st_value = prev_symbol['st_value'] & ~1 \
                if prev_symbol['st_info']['type'] == 'STT_FUNC' else prev_symbol['st_value']
            prev_st_size = prev_symbol['st_size']
            if st_value - prev_st_value != prev_st_size:
                gap_size = st_value - prev_st_value - prev_st_size
                gap_addr = prev_st_value + prev_st_size
                if gap_addr in text_range:
                    start = gap_addr - text_base
                    end = start + gap_size
                    fn_code = code[start:end]
                elif data_section and gap_addr in data_range:
                    start = gap_addr - data_base
                    end = start + gap_size
                    fn_code = data[start:end]
                else:
                    assert False

                sym = Symbol(gap_addr, gap_size, fn_code, "gap%d" % gap_cnt)
                setattr(sym, "type", "STT_OBJECT")
                setattr(sym, "anonymous", True)
                fn_symbols.append(sym)
                gap_cnt += 1

        if s.name != '__aeabi_memcpy':
            if st_size == 0 and i < len(raw_symbols) - 1:
                st_size = raw_symbols[i + 1]['st_value'] - st_value
                if raw_symbols[i + 1]['st_info']['type'] == 'STT_FUNC':
                    st_size -= 1
        else:
            st_size = 52

        if s['st_value'] in text_range:
            start = st_value - text_base
            if s['st_info']['type'] == 'STT_FUNC':
                st_value += 1
            end = start + st_size
            fn_code = code[start:end]
        elif data_section and s['st_value'] in data_range:
            start = st_value - data_base
            end = start + st_size
            fn_code = data[start:end]
        else:
            assert False

        if s['st_info']['type'] == 'STT_NOTYPE':
            st_type = 'STT_OBJECT'
        else:
            st_type = s['st_info']['type']

        if (strtab.get_string(s['st_name']) == '__sys_write'):
            assert False

        sym = Symbol(st_value, st_size, fn_code, strtab.get_string(s['st_name']))
        setattr(sym, "type", st_type)
        fn_symbols.append(sym)
        prev_symbol = s

        if i == len(raw_symbols) - 1:
            if len(code[end:]) > 0:
                st_value = end + text_base
                st_size = len(code[end:])
                fn_code = code[end:]
                sym = Symbol(st_value, st_size, fn_code, "gap%d" % gap_cnt)
                setattr(sym, "type", st_type)
                setattr(sym, "anonymous", True)
                fn_symbols.append(sym)

        fn_symbols.sort(key=lambda x: x.address)

    return fn_symbols, etext

if __name__ == "__main__":
    pass