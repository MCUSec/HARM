from .ir import IR


class NopIR(IR):
    def __init__(self, offset, wide=False, parent=None):
        super().__init__(offset, None, parent)
        self.__wide = wide

    def __repr__(self):
        return "%s: No operation (NOP)" % hex(self.addr)

    def __str__(self):
        if len(self._code) > 0:
            return super().__str__()
        else:
            return self.__repr__()

    @property
    def wide(self):
        return self.__wide

    @wide.setter
    def wide(self, v):
        assert isinstance(v, bool)
        self.__wide = v

    @property
    def len(self):
        return 2 if not self.wide else 4

    def asm(self):
        if not self.__wide:
            code, count = IR._ks.asm("nop")
        else:
            code, count = IR._ks.asm("nop.w")
        assert len(code) == self.len
        self._code = bytearray(code)

