from keystone import KsError

from .arm_reg import ArmReg
from .ir import IR


class RegisterToRegisterIR(IR):
    def __init__(self, dest_reg, src_reg, setflags=False, offset=0, parent=None):
        super().__init__(offset=offset, parent=parent)
        assert isinstance(dest_reg, ArmReg) and isinstance(src_reg, ArmReg)
        self.__dest_reg = dest_reg
        self.__src_reg = src_reg
        self.__setflags = setflags

    def __repr__(self):
        return "%s: %s = %s" % (hex(self.addr), self.__dest_reg, self.__src_reg)

    def __str__(self):
        if len(self._code):
            return super().__str__()
        else:
            return self.__repr__()

    def asm(self):
        asmcode = ("mov %s, %s" if not self.__setflags else "movs %s, %s") % (self.__dest_reg, self.__src_reg)
        code, count = IR._ks.asm(asmcode)
        try:
            code, count = IR._ks.asm(asmcode)
        except KsError:
            print(asmcode)
        if len(code) != self.len:
            print(len(code))
            print(asmcode)
        assert len(code) == self.len
        self._code = bytearray(code)

    @property
    def len(self):
        if self.__setflags and str(self.__dest_reg) in ('r8', 'r9', 'r10', 'r11', 'r12', 'sp', 'lr', 'pc'):
            return 4
        else:
            return 2