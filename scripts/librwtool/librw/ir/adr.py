import warnings

from .arm_reg import ArmReg
from .ref import *


class AddressToRegisterIR(RefIR):
    def __init__(self, reg=None, offset=0, parent=None):
        super().__init__(offset, parent)
        if reg:
            assert isinstance(reg, ArmReg)
            self.__reg = reg

    def __repr__(self):
        if self.ref:
            return "%s: %s = %s (%s) adr" % (hex(self.addr), self.reg, hex(self.ref.addr), self.ref)
        else:
            return "%s: %s = %s (%s) adr" % (hex(self.addr), self.reg, hex(self.ref_addr), self.ref)

    def __str__(self):
        # if len(self._code):
        #     return super().__str__()
        # else:
        #     return self.__repr__()
        return self.__repr__()

    @property
    def reg(self):
        return self.__reg

    @reg.setter
    def reg(self, reg):
        assert isinstance(reg, ArmReg)
        self.__reg = reg

    def __calc_disp(self):
        pc = (self.addr + 4) & ~3
        return self.ref.addr - pc

    def reachable(self):
        disp = self.__calc_disp()
        if not self.wide:
            return disp >= 0 and (disp >> 2) == (disp >> 2) & 0xff
        else:
            disp = abs(disp)
            return disp == disp & 0xfff

    def asm(self):
        if self.ref.addr % 4 != 0:
            warnings.warn("%s is not aligned! (%s)" % (self, self.parent))
        if self.reachable():
            disp = self.__calc_disp()
            asmcode = "adr %s, %d" % (self.reg, disp) \
                if not self.wide else "adr.w %s, %d" % (self.reg, disp)
            try:
                code, count = IR._ks.asm(asmcode)
            except KsError:
                print(asmcode)
            assert len(code) == self.len
            self._code = bytearray(code)
        else:
            raise ValueError("Out of range")

