from .ir import IR
from copy import deepcopy


class BlockIR(IR):
    def __init__(self, offset=0, init_pos=0, parent=None):
        super().__init__(offset, None, parent)
        self._init_pos = init_pos
        self._pos = init_pos
        self._child = list()
        self._len = 0

    def __repr__(self):
        irstr = ""
        for i in self._child:
            irstr += i.__repr__() + "\n"
        return irstr.rstrip()

    def __str__(self):
        irstr = ""

        if len(self._code) > 0:
            irstr += super().__str__() + "\n"

        for i in self._child:
            irstr += i.__str__() + "\n"

        return irstr.rstrip()

    @property
    def code(self):
        c = deepcopy(self._code)
        for i in self._child:
            c += i.code
        return c

    @property
    def size(self):
        return len(self._child)

    @property
    def len(self):
        length = 0
        for i in self._child:
            length += i.len
        return length

    def __stretch(self, where, size):
        for i in self._child[where:]:
            i.offset += size
        self._pos += size
        self._len += size

    def stretch(self, ir, stretch_size):
        if stretch_size != 0:
            idx = self._child.index(ir) + 1
            self.__stretch(idx, stretch_size)

    def append_child(self, ir):
        ir.parent = self
        ir.offset = self._pos
        self._child.append(ir)
        self._pos += ir.len
        self._len += ir.len

    def insert_child(self, src_ir, new_ir, pos='before'):
        assert pos.lower() in ('before', 'after', 'replace')
        assert src_ir in self._child

        new_ir.parent = self
        idx = self._child.index(src_ir)
        if pos != 'replace':
            if pos.lower() == 'after':
                if src_ir is self._child[-1]:
                    self.append_child(new_ir)
                    return
                idx += 1

            new_ir.offset = self._child[idx].offset
            self.__stretch(idx, new_ir.len)
            self._child.insert(idx, new_ir)
        else:
            new_ir.offset = src_ir.offset
            self._child[idx] = new_ir
            if new_ir.len != src_ir.len:
                self.__stretch(idx + 1, new_ir.len - src_ir.len)

    def layout_refresh(self):
        orig_size = self._len
        self._child = list(filter(lambda x: not hasattr(x, "void") and x.parent is self, self._child))
        self._pos = self._init_pos
        self._len = 0
        for i in self._child:
            i.offset = self._pos
            if hasattr(i, "layout_refresh"):
                i.layout_refresh()
            self._pos += i.len
            self._len += i.len
        return self._len - orig_size

    def child_iter(self):
        for i in self._child:
            yield i

    def child_index(self, ir):
        return self._child.index(ir)

    def child_at(self, index):
        return self._child[index]

    def asm(self):
        for i in self._child:
            i.asm()


class BlockInsertError(Exception):
    pass




