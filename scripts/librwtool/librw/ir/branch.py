from .function import FunctionIR
from .ref import *
from .it_block import *
from .utils import *


class BranchIR(RefIR):
    def __init__(self, offset, link=False, parent=None):
        super().__init__(offset, parent)
        self.__link = link

    def __repr__(self):
        if self.link:
            if isinstance(self.ref, FunctionIR):
                return "%s: call function %s @ %s" % (hex(self.addr), self.ref.name, hex(self.ref.addr))
            elif self.ref:
                return "%s: branch and link to address %s" % (hex(self.addr), hex(self.ref.addr))
            else:
                return "%s: branch and link to address %s" % (hex(self.addr), hex(self.ref_addr))
        else:
            if isinstance(self.ref, FunctionIR):
                return "%s: branch to function %s @ %s" % (hex(self.addr), self.ref.name, hex(self.ref.addr))
            elif self.ref:
                return "%s: branch to address %s (%s+%s)" % (hex(self.addr), hex(self.ref.addr),
                                                             self.ref.parent.name, hex(self.ref.offset))
            else:
                return "%s: branch to address %s" % (hex(self.addr), hex(self.ref_addr))

    def __str__(self):
        if len(self.code) > 0:
            return "%s (%s)" % (super().__str__(), self.__repr__())
        else:
            return self.__repr__()

    @property
    def link(self):
        return self.__link

    @link.setter
    def link(self, v):
        assert isinstance(v, bool)
        self.__link = v
        if self.__link:
            self.len = 4

    def __calc_disp(self):
        return abs(self.ref.addr - self.addr - 4)

    def reachable(self):
        disp = self.ref.addr - self.addr
        if not self.__link:
            if self.cond == ARM_CC_AL or isinstance(self.parent, ITBlockIR):
                # for encoding T2 and T4
                if self.len == 2:
                    return -2044 <= disp <= 2046
                else:
                    return -16777216 <= disp <= 16777214
            else:
                # for encoding T1 and T3
                if self.len == 2:
                    return -252 <= disp <= 254
                else:
                    return -1048576 <= disp <= 1048574
        else:
            return True

    def _verify(self, asmcode):
        """
        !!!! Bugs might exist in keystone, disassemble and verify !!!!
        """
        for inst in self._md.disasm(bytes(self._code), self.addr):
            if inst.id in (ARM_INS_B, ARM_INS_BL) and inst.operands[0].imm != self.ref.addr:
                print("!!!! %s: %s\t%s !!!!" % (hex(inst.address), inst.mnemonic, inst.op_str))
                print(repr(self))
                print(asmcode)
                for i in bytes(self._code):
                    print(hex(i))
                assert 1 == 0

            if not isinstance(self.parent, ITBlockIR):
                assert self.cond == inst.cc

    def asm(self):
        if self.reachable():
            if not self.__link:
                if self.cond == ARM_CC_AL or isinstance(self.parent, ITBlockIR):
                    asmcode = ("b #%s" if not self.wide else "b.w #%s") % hex(self.ref.addr)
                else:
                    if self.wide:
                        asmcode = "b%s.w #%s" % (get_cond_name(self.cond), hex(self.ref.addr))
                    else:
                        asmcode = "b%s #%s" % (get_cond_name(self.cond), hex(self.ref.addr))
            else:
                asmcode = "bl #%s" % hex(self.ref.addr)
            code, count = IR._ks.asm(asmcode, addr=self.addr)
            if len(code) != self.len:
                print(repr(self))
                print(asmcode)
                print(len(code))
                print(self.len)
            assert len(code) == self.len
            self._code = bytearray(code)
            self._verify(asmcode)
        else:
            raise ValueError("Out of range")






