from .ir import *
from .arm_reg import *


class IndirectBranchIR(IR):
    def __init__(self, reg=ARM_REG_R0, link=False, offset=0, parent=None):
        super().__init__(offset, None, parent)
        self.__reg = ArmReg(reg)
        self.__link = link

    def __repr__(self):
        if self.link:
            return "%s: Indirect branch and link (specified by %s)" % (hex(self.addr), self.reg)
        else:
            return "%s: Indirect branch (specified by %s)" % (hex(self.addr), self.reg)

    def __str__(self):
        if len(self._code) > 0:
            return super().__str__()
        else:
            return self.__repr__()

    @property
    def link(self):
        return self.__link

    @link.setter
    def link(self, v):
        assert isinstance(v, bool)
        self.__link = v

    @property
    def reg(self):
        return self.__reg

    @reg.setter
    def reg(self, v):
        assert isinstance(v, int)
        self.__reg.id = v

    @property
    def len(self):
        return 2

    def asm(self):
        asmcode = "bx %s" % self.__reg if not self.link else "blx %s" % self.__reg
        code, count = IR._ks.asm(asmcode)
        self._code = bytearray(code)


