from copy import copy

from capstone.arm import *
from keystone import KsError

from .arm_reg import ArmReg
from .block import BlockIR
from .ir import IR
from .ref import RefIR
from .utils import tohex


class LoadBranchTableIR(RefIR):
    def __init__(self, top=False, reg=ArmReg(ARM_REG_R0), ref=None, offset=0, parent=None):
        super().__init__(offset, parent)
        self._ref = ref
        self._len = 4
        self.__reg = reg
        self.__top = top

    def __repr__(self):
        if self.__top:
            return "%s: Load Branch Table High Address (%s)" % (hex(self.addr), hex(self.ref.addr))
        else:
            return "%s: Load Branch Table Low Address (%s)" % (hex(self.addr), hex(self.ref.addr))

    def __str__(self):
        if len(self.code) > 0:
            return "%s (%s)" % (super().__str__(), self.__repr__())
        else:
            return self.__repr__()

    @property
    def reg(self):
        return copy(self.__reg)

    @reg.setter
    def reg(self, v):
        assert isinstance(v, ArmReg)
        self.__reg = copy(v)

    def __calc_disp(self):
        pass

    def reachable(self):
        return True

    def asm(self):
        if self.__top:
            asmcode = "movt %s, %s" % (self.__reg, hex(self.ref.addr >> 16))
        else:
            asmcode = "movw %s, %s" % (self.__reg, hex(self.ref.addr & 0xFFFF))
        try:
            code, count = IR._ks.asm(asmcode)
        except KsError:
            print(asmcode)
        assert len(code) == self.len
        self._code = bytearray(code)

class TableBranchIR(IR):
    def __init__(self, offset, code, parent=None):
        super().__init__(offset, code, parent)
        self.__entry_size = 1

    def __repr__(self):
        return ("%s: Table Branch Byte (TBB)" if self.__entry_size == 1 else "%s: Table Branch Halfword (TBH)") \
               % hex(self.addr)

    def __str__(self):
        if len(self.code) > 0:
            return "%s (%s)" % (super().__str__(), self.__repr__())
        else:
            return self.__repr__()

    @property
    def entry_size(self):
        return self.__entry_size

    @entry_size.setter
    def entry_size(self, v):
        assert isinstance(v, int)
        assert v in (1, 2)
        self.__entry_size = v

    @property
    def len(self):
        return 4


class BranchTableIR(BlockIR):
    entry_size = 4
    ref_by_load = None
    ref_by_branch = None

    def __init__(self, entry_size=4, ref_by_load=None, ref_by_branch=None, offset=0, parent=None):
        super().__init__(offset=offset, parent=parent)
        self.__entry_size = entry_size
        self.__ref_by_load = ref_by_load
        self.__ref_by_branch = ref_by_branch

    @property
    def ref_by_load(self):
        return self.__ref_by_load

    @ref_by_load.setter
    def ref_by_load(self, v):
        self.__ref_by_load = v

    @property
    def ref_by_branch(self):
        return self.__ref_by_branch

    @ref_by_branch.setter
    def ref_by_branch(self, v):
        self.__ref_by_branch = v


class TableBranchEntryIR(RefIR):
    def __init__(self, offset=0, length=4, parent=None):
        super().__init__(offset, parent)
        self.__enforce_offset = False
        self._len = length

    def __repr__(self):
        if self._len == 1:
            return "%s: jump to %s (.byte 0x%02x)" % (hex(self.addr), hex(self.ref_addr), self.value)
        elif self._len == 2:
            return "%s: jump to %s (.short 0x%04x)" % (hex(self.addr), hex(self.ref_addr), self.value)
        else:
            return "%s: jump to %s (.word 0x%08x)" % (hex(self.addr), hex(self.ref_addr), self.value)

    def __str__(self):
        return self.__repr__()

    @property
    def enforce_use_offset(self):
        return self.__enforce_offset

    @enforce_use_offset.setter
    def enforce_use_offset(self, v):
        assert isinstance(v, bool)
        self.__enforce_offset = v

    @property
    def len(self):
        return self._len

    @len.setter
    def len(self, v):
        assert isinstance(v, int)
        self._len = v

    @property
    def value(self):

        if self.len == 4:
            if self.__enforce_offset:
                return tohex(self.ref.addr - (self.parent.ref_by_branch.addr + 8) + 1, 32)
            else:
                return self.parent.addr + self.__calc_disp() + 1
        else:
            return tohex(self.__calc_disp(), 32) >> 1

        # if self.len != 4:
        #     return tohex(self.__calc_disp(), 32) >> 1
        # elif self.__enforce_offset:
        #     # return (self.ref.addr - self.parent.parent.addr) >> 1
        #     return self.ref.addr - (self.parent.ref_by_branch + 8)
        # else:
        #     return self.parent.addr + self.__calc_disp() + 1

    def __calc_disp(self):
        return self.ref.addr - self.parent.addr

    def reachable(self):
        disp = tohex(self.__calc_disp(), 32)
        if self.len == 1:
            return (disp >> 1) <= 0xff
        elif self.len == 2:
            return (disp >> 1) <= 0xffff
        else:
            return True

    def asm(self):
        if self.reachable():
            disp = self.__calc_disp()
            if self.len != 4:
                target = disp >> 1
            elif self.__enforce_offset:
                target = tohex(self.ref.addr - (self.parent.ref_by_branch.addr + 8) + 1, 32)
                # target = self.ref.addr - self.parent.parent.addr
            else:
                target = self.parent.addr + disp + 1
            self._code = bytearray(target.to_bytes(self.len, byteorder='little'))
        else:
            raise ValueError("Out of range")

