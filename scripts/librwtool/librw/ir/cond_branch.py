from .ref import *
from .arm_reg import *


class CondBranchIR(RefIR):
    """
    CBNZ, CBZ
    """
    def __init__(self, offset, parent=None):
        super().__init__(offset, parent)
        self.__reg = None
        self.__zf = False

    def __repr__(self):
        if self.zero_flag:
            return "%s: branch to address %s when %s == 0" % (hex(self.addr), hex(self.ref_addr), self.reg)
        else:
            return "%s: branch to address %s when %s != 0" % (hex(self.addr), hex(self.ref_addr), self.reg)

    def __str__(self):
        if len(self.code) > 0:
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
    def zero_flag(self):
        return self.__zf

    @zero_flag.setter
    def zero_flag(self, v):
        assert isinstance(v, bool)
        self.__zf = v

    def __calc_disp(self):
        return abs(self.ref.addr - self.addr - 4)

    def asm(self):
        if self.reachable():
            if self.len == 2:
                asmcode = ("cbz %s, #%d" if self.__zf else "cbnz %s, #%d") % (self.reg, self.ref.addr)
            elif self.len == 4:
                asmcode = ("cmp %s, #0; beq #%d" if self.__zf else "cmp %s, #0; bne #%d") % (self.reg, self.ref.addr)
            else:
                asmcode = ("cmp %s, #0; beq.w #%d" if self.__zf else "cmp %s, #0; bne.w #%d") % \
                          (self.reg, self.ref.addr)
            code, count = IR._ks.asm(asmcode, addr=self.addr)
            assert len(code) == self.len
            self._code = bytearray(code)

            """
            !!!! Bugs might exist in keystone, disassemble and verify !!!!
            """
            for inst in self._md.disasm(bytes(self.code), self.addr):
                if inst in (ARM_INS_CBNZ, ARM_INS_CBZ):
                    if inst.operands[1].imm != self.ref.addr:
                        print("!!!! %s: %s\t%s !!!!" % (hex(inst.address), inst.mnemonic, inst.op_str))
                        print(repr(self))
                    assert inst.operands[1].imm == self.ref.addr
                elif inst == ARM_INS_B:
                    if inst.operands[0].imm != self.ref.addr:
                        print("!!!! %s: %s\t%s !!!!" % (hex(inst.address), inst.mnemonic, inst.op_str))
                        print(repr(self))
                    assert inst.operands[0].imm == self.ref.addr
                else:
                    pass
        else:
            raise ValueError("Out of range")

    def reachable(self):
        disp = self.ref.addr - self.addr
        if self.len == 2:
            # cbz / cbnz
            return 0 <= disp <= 126
        elif self.len == 4:
            # cmp; beq/bne
            return -256 <= disp <= 254
        else:
            # cmp; beq.w/bne.w (always reachable)
            return True


