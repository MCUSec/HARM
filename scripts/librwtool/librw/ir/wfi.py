from librw.ir.ir import IR


class WfiIR(IR):
    def __init__(self, offset=0, parent=None):
        super().__init__(offset, None, parent)

    def __repr__(self):
        return "%s: Wait for interrupt" % hex(self.addr)

    def __str__(self):
        if len(self._code) > 0:
            return super().__str__()
        else:
            return self.__repr__()

    @property
    def len(self):
        return 2

    def asm(self):
        code, count = IR._ks.asm("wfi")
        assert len(code) == self.len
        self._code = bytearray(code)

