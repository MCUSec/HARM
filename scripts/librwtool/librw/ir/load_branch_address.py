from capstone.arm_const import *
from keystone import KsError

from .arm_reg import ArmReg
from .ir import IR
from .table_branch import BranchTableIR


class LoadBranchAddressIR(IR):
    def __init__(self, base_reg=None, index_reg=None, offset=0, parent=None):
        super().__init__(offset=offset, parent=parent)
        self.__dest_reg = ArmReg(ARM_REG_PC)
        self.__base_reg = base_reg
        self.__index_reg = index_reg
        self.__branch_table = None

    @property
    def dest_reg(self):
        return self.__dest_reg

    @dest_reg.setter
    def dest_reg(self, v):
        assert isinstance(v, ArmReg)
        self.__dest_reg = v

    @property
    def base_reg(self):
        return self.__base_reg

    @base_reg.setter
    def base_reg(self, v):
        assert isinstance(v, ArmReg)
        self.__base_reg = v

    @property
    def index_reg(self):
        return self.__index_reg

    @index_reg.setter
    def index_reg(self, v):
        assert isinstance(v, ArmReg)
        self.__index_reg = v

    @property
    def len(self):
        return 4

    @property
    def branch_table(self):
        return self.__branch_table

    @branch_table.setter
    def branch_table(self, v):
        assert isinstance(v, BranchTableIR)
        self.__branch_table = v

    def asm(self):
        asmcode = "ldr %s, [%s, %s, lsl #2]" % (self.dest_reg, self.base_reg, self.index_reg)
        try:
            code, count = IR._ks.asm(asmcode)
        except KsError:
            print(asmcode)
        assert len(code) == self.len
        self._code = bytearray(code)