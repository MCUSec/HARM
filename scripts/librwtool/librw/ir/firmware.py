from .block import BlockIR
from .branch import BranchIR
from .function import FunctionIR, NopIR
from .ir import IR
from .object import ObjectIR


class FirmwareIR(BlockIR):
    def __init__(self, name, offset=0):
        super().__init__(offset)
        self.__name = name
        self.fn_map = dict()

    @property
    def name(self):
        return self.__name

    @property
    def parent(self):
        return None

    def append_child(self, fn: IR, align=2):
        assert align in (2, 4)
        super().append_child(fn)

    def commit(self):
        fn = filter(lambda x: isinstance(x, FunctionIR), self._child)
        for i in fn:
            for ir in i.child_iter():
                if isinstance(ir, BranchIR) and not ir.ref:
                    if ir.ref_addr in self.fn_map:
                        ir.ref = self.fn_map[ir.ref_addr]["ir"]
                    else:
                        for k in self.fn_map:
                            if k < ir.ref_addr <= k + self.fn_map[k]["ir"].len:
                                ir.ref = self.fn_map[k]["ir"].get_ir(ir.ref_addr - k)
                        if not ir.ref:
                            print(ir)
                        assert ir.ref

    def layout_refresh(self):
        pads = list()
        pad_index = 0
        total_padding = 0
        self._pos = self._init_pos

        for ir in self._child:
            ir.offset = self._pos
            if isinstance(ir, ObjectIR) and (self._pos + self.addr + total_padding) % ir.align != 0:
                pad_size = ir.align - ((self._pos + self.addr + total_padding) & (ir.align - 1))
                pads.append({"ir": ir, "size": pad_size})
                total_padding += pad_size

                print("%s, pad_size: %d" % (ir, pad_size))
            else:
                if hasattr(ir, "layout_refresh"):
                    ir.layout_refresh()

            self._pos += ir.len

        for pad in pads:
            self.insert_child(pad["ir"], ObjectIR(name="pad%d" % pad_index, code=b"\xff" * pad["size"], align=1))
            pad_index += 1

    def verify(self):
        pos = 0
        for i in self._child:
            if hasattr(i, "verify"):
                i.verify()

            if i.offset != pos:
                print("%s, offset is incorrect! (offset=%s, pos=%s, align=%d)" % (i, hex(i.offset), hex(pos), i.align))
                assert False
            else:
                pos += i.len

    def output_to_stream(self, ir, stream):
        if not hasattr(ir, "child_iter"):
            stream.write(ir.code)
            return len(ir.code)
        else:
            for i in ir.child_iter():
                return self.output_to_stream(i, stream)

    def save_as_file(self, path):
        with open(path, "wb") as stream:
            stream.write(self.code)
