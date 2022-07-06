from capstone.arm import *
from .utils import *
from .ir import *


class PopIR(IR):
    def __init__(self, offset, parent=None):
        super().__init__(offset, None, parent)
        self.__regs = set()
        self.__wide = False

    def __repr__(self):
        return "%s: POP return address" % hex(self.addr)

    def __str__(self):
        if len(self._code) > 0:
            return super().__str__()
        else:
            return self.__repr__()

    @property
    def wide(self):
        s = {ARM_REG_R8, ARM_REG_R9, ARM_REG_R10, ARM_REG_R11, ARM_REG_R12, ARM_REG_LR}
        return len(s & self.__regs) > 0

    @property
    def len(self):
        return 2 if not self.wide else 4

    @property
    def reg_list(self):
        return self.__regs

    @reg_list.setter
    def reg_list(self, v):
        assert isinstance(v, set)
        self.__regs = v

    def add_reg(self, reg):
        self.__regs.add(reg)

    def remove_reg(self, reg):
        if reg in self.__regs:
            self.__regs.remove(reg)

    def asm(self):
        asmcode = list("pop {")
        lst = list(self.__regs)
        lst.sort()
        for i in lst:
            asmcode += get_register_name(i)
            asmcode += ","
        asmcode[-1] = '}'

        code, count = IR._ks.asm(''.join(asmcode))
        assert len(code) == self.len
        self._code = bytearray(code)
