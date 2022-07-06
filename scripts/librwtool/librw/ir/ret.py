from .ir import *


class ReturnIR(IR):
    def __init__(self, offset, parent=None):
        super().__init__(offset, None, parent)

    def __repr__(self):
        return "%s: PC = Mem[SP]" % hex(self.addr)

    def __str__(self):
        if len(self._code) > 0:
            return super().__str__()
        else:
            return self.__repr__()

    @property
    def len(self):
        return 4

    def asm(self):
        asmcode = "ldr pc, [sp], #4"
        code, count = IR._ks.asm(asmcode)
        assert len(code) == self.len
        self._code = bytearray(code)