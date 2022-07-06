import warnings

from .ref import *
from .arm_reg import *


class LoadLiteralIR(RefIR):
    def __init__(self, offset=0, float_reg=False, parent=None):
        super().__init__(offset, parent)
        self.__reg = None
        self.__float = float_reg

    def __repr__(self):
        return "%s: %s = Mem[%s] (%s)" % (hex(self.addr), self.reg, hex(self.ref_addr), self.ref)

    def __str__(self):
        if len(self._code):
            return super().__str__()
        else:
            return self.__repr__()

    @property
    def reg(self):
        return self.__reg

    @reg.setter
    def reg(self, v):
        assert isinstance(v, ArmReg)
        self.__reg = v

    @property
    def float_reg(self):
        return self.__float

    @float_reg.setter
    def float_reg(self, v):
        assert isinstance(v, bool)
        self.__float = v

    def __calc_disp(self):
        pc = (self.addr + 4) & ~3
        return self.ref.addr - pc

    def asm(self):
        if self.ref.addr % 4 != 0:
            warnings.warn("%s is not aligned! (%s)" % (self, self.parent))
        if self.reachable():
            disp = self.__calc_disp()
            if not self.float_reg:
                asmcode = "ldr %s, [pc, #%d]" % (self.reg, disp) \
                    if not self.wide else "ldr.w %s, [pc, #%d]" % (self.reg, disp)
            else:
                asmcode = "vldr %s, [pc, #%d]" % (self.reg, disp)
            try:
                code, count = IR._ks.asm(asmcode)
            except KsError:
                print(asmcode)
            if len(code) != self.len:
                print(asmcode)
            assert len(code) == self.len
            self._code = bytearray(code)
        else:
            raise ValueError("Out of range")

    def reachable(self):
        disp = self.__calc_disp()
        if not self.wide:
            return disp >= 0 and (disp >> 2) == (disp >> 2) & 0xff
        else:
            disp = abs(disp)
            return disp == disp & 0xfff