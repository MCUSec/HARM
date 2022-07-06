from .it_block import *
from .nop import *
from .ref import RefIR
from .table_branch import BranchTableIR


class FunctionIR(BlockIR):
    def __init__(self, name, offset=0, parent=None):
        super().__init__(offset, init_pos=0, parent=parent)
        self.__name = name
        self.__ir_map = dict()
        self.isr = False

    def __repr__(self):
        if not self.isr:
            return "%s @ %s (size: %s)" % (self.__name, hex(self.addr), hex(self.len))
        else:
            return "%s @ %s IRQ (size: %s)" % (self.__name, hex(self.addr), hex(self.len))

    def __str__(self):
        return self.__repr__()

    @property
    def name(self):
        return self.__name

    def __update_ir_map(self):
        self.__ir_map.clear()
        for i in self._child:
            self.__ir_map[i.offset] = i
            if not isinstance(i, BlockIR):
                self.__ir_map[i.offset] = i
            else:
                if isinstance(i, ITBlockIR):
                    self.__ir_map[i.offset] = i
                for j in i.child_iter():
                    self.__ir_map[i.offset + j.offset] = j

    def layout_refresh(self):
        children = self._child
        finished = False
        orig_size = self._len

        while not finished:
            # remove all NOP IR except those referenced by other IR
            children = list(filter(lambda x: (not isinstance(x, NopIR) or hasattr(x, "ref_by") or
                                              self.name == "SVC_Handler" or self.name == "vStartFirstTask") and
                                             not hasattr(x, "void") and x.parent is self, children))
            where_nop = list()
            self._pos = 0
            # assign offset for each instruction
            for i in children:
                i.offset = self._pos
                if isinstance(i, BlockIR) and not isinstance(i, ITBlockIR) and \
                        (not isinstance(i, BranchTableIR) or i.entry_size == 4) and i.addr % 4 != 0:
                    i.layout_refresh()
                    # Align literal pool to 4 bytes
                    where_nop.append((children.index(i), self._pos))
                    self._pos += 2
                    i.offset = self._pos
                self._pos += i.len

            # insert NOP instructions to adjust the layout
            for idx, offset in where_nop:
                children.append(NopIR(offset, parent=self))

            children.sort(key=lambda x: x.offset)

            # check ref instructions is reachable
            ref_ir = list(filter(lambda x: isinstance(x, RefIR), children))
            if len(ref_ir) > 0:
                for i in ref_ir:
                    if not i.reachable():
                        i.len += 2
                        break
                    if i is ref_ir[-1]:
                        finished = True
            else:
                finished = True

        self._child = children
        self._len = self._pos
        self.__update_ir_map()
        return self._len - orig_size

    def verify(self):
        pos = 0
        for i in self._child:
            if i.offset != pos:
                print("%s ir offset is incorrect!" % i)
                assert 1 == 0
            else:
                pos += i.len
        from .branch import BranchIR
        from .cond_branch import CondBranchIR
        for i in self._child:
            if isinstance(i, BranchIR) or isinstance(i, CondBranchIR):
                if i.ref.parent == self:
                    if i.ref not in self._child:
                        print(i)
                        print(i.ref)
                    assert i.ref in self._child
                if i.ref.parent == self and (
                        i.ref.offset not in self.__ir_map or self.__ir_map[i.ref.offset] is not i.ref):
                    # assert i.ref in self._child
                    if i.ref.offset in self.__ir_map:
                        print(self.__ir_map[i.ref.offset])
                    else:
                        print("%s not in map" % i.ref)
                    print("aa: %s" % i)
                    print("bb: %s" % i.ref)
                    print(self)
                    # assert 1 == 0

    def append_child(self, ir):
        super().append_child(ir)
        self.__ir_map[ir.offset] = ir
        if isinstance(ir, BlockIR):
            for i in ir.child_iter():
                self.__ir_map[ir.offset + i.offset] = i
        # for k in self.__ir_map:
        #     i = self.__ir_map[k]
        #     if isinstance(i, RefIR) and not i.ref and i.ref_addr in self.__ir_map:
        #         i.ref = self.__ir_map[i.ref_addr]

    # def insert_child(self, src_ir, new_ir, pos='before'):
    #     super().insert_child(src_ir, new_ir, pos)
    #     self.__update_ir_map()

    def __commit(self, ir):
        if isinstance(ir, BlockIR):
            for i in ir.child_iter():
                self.__commit(i)
        else:
            if isinstance(ir, RefIR) and not ir.ref and ir.ref_addr in self.__ir_map:
                ir.ref = self.__ir_map[ir.ref_addr]

    def commit(self):
        self.__update_ir_map()
        for ir in self._child:
            self.__commit(ir)

    def get_ir(self, offset):
        return self.__ir_map[offset]

    def output_yaml_desp(self, idx):
        desp = {
            "index":  idx,
            "name": self.name,
            "kind": "Function",
            "address": self.addr,
            "size": self.len,
            "isr": 0,
            "reloc_items": list()
        }
        return desp
