from .arm_reg import ArmReg
from .function import FunctionIR
from .ir import IR


class LoadReturnIndexIR(IR):
    count = 0

    def __init__(self, caller_index, offset=0, parent=None):
        super().__init__(offset, None, parent)
        self._id = LoadReturnIndexIR.count
        self._caller_index = caller_index
        LoadReturnIndexIR.count += 1

    def __repr__(self):
        return "%s: LR = %d" % (hex(self.addr), self.id)

    def __str__(self):
        if len(self._code) > 0:
            return super().__str__()
        else:
            return self.__repr__()

    @property
    def id(self):
        return self._id

    @property
    def ret_offset(self):
        if not isinstance(self.parent, FunctionIR):
            return self.parent.offset + self._offset + 8
        else:
            return self._offset + 8

    @property
    def caller_index(self):
        return self._caller_index

    @property
    def len(self):
        return 4

    @property
    def encode(self):
        # return (((1 << 12) | self.caller_index) << 16) | (self.ret_offset + 1)
        return (self.caller_index << 16) | self.ret_offset

    def asm(self):
        asmcode = "movw lr, #%d" % ((self.id << 1) | 1)
        code, count = IR._ks.asm(asmcode)
        assert len(code) == self.len
        self._code = bytearray(code)


class LoadReturnOffsetIR(IR):
    def __init__(self, offset=0, parent=None):
        super().__init__(offset, None, parent)

    def __repr__(self):
        return "%s: LR[15:0] = %s" % (hex(self.addr), hex(self.ret_offset))

    def __str__(self):
        if len(self._code) > 0:
            return super().__str__()
        else:
            return self.__repr__()

    @property
    def len(self):
        return 4

    @property
    def ret_offset(self):
        if not isinstance(self.parent, FunctionIR):
            return self.parent.offset + self._offset + 12
        else:
            return self._offset + 12

    def asm(self):
        asmcode = "movw lr, #%s" % hex(self.ret_offset + 1)
        code, count = IR._ks.asm(asmcode)
        assert len(code) == self.len
        self._code = bytearray(code)


class LoadCallerIndexIR(IR):
    def __init__(self, idx, reg, offset=0, parent=None):
        super().__init__(offset, None, parent)
        self.__caller_index = idx
        assert isinstance(reg, ArmReg)
        self.__reg = reg

    def __repr__(self):
        return "%s: LR[31:16] = %d" % (hex(self.addr), self.caller_index)

    def __str__(self):
        if len(self.code) > 0:
            return super().__str__()
        else:
            return self.__repr__()

    @property
    def len(self):
        return 4

    @property
    def caller_index(self):
        return self.__caller_index

    @caller_index.setter
    def caller_index(self, v):
        self.__caller_index = v

    @property
    def reg(self):
        return self.__reg

    @reg.setter
    def reg(self, v):
        assert isinstance(v, ArmReg)
        self.__reg = v

    def asm(self):
        # print(self.__reg.id)
        asmcode = "movt %s, #%d" % (str(self.reg.id), self.caller_index)
        code, count = IR._ks.asm(asmcode)
        if len(code) != self.len:
            print(asmcode)
        assert len(code) == self.len
        self._code = bytearray(code)


class LoadFuncPtrIR(IR):
    def __init__(self, reg, offset=0, parent=None):
        super().__init__(offset, None, parent)
        self.__reg = reg

    def __repr__(self):
        return "%s: R12 = %s" % (hex(self.addr), self.reg)

    def __str__(self):
        if len(self.code) > 0:
            return super().__str__()
        else:
            return self.__repr__()

    @property
    def len(self):
        return 2

    @property
    def reg(self):
        return self.__reg

    @reg.setter
    def reg(self, v):
        assert isinstance(v, ArmReg)
        self.__reg = v

    def asm(self):
        asmcode = "mov r12, %s" % self.reg
        code, count = IR._ks.asm(asmcode)
        assert len(code) == self.len
        self._code = bytearray(code)


class RetEncodeIR(IR):
    def __init__(self, offset=0, parent=None):
        super().__init__(offset, None, parent)
        self.__caller_index = 0
        self.__reg = None
        self._len = 8

    def __str__(self):
        if len(self.code) > 0:
            return super().__str__()

        lr = (self.caller_index << 4) | self.ret_offset
        if not self.reg:
            return "%s: Let LR = %s (caller_index: %d, return offset: %s)" % \
                   (hex(self.addr), hex(lr), self.caller_index, hex(self.ret_offset))
        else:
            return "%s: Let R12=%s, LR = %s (caller_index: %d, return offset: %s)" % \
                   (hex(self.addr), self.reg, hex(lr), self.caller_index, hex(self.ret_offset))

    @property
    def reg(self):
        return self.__reg

    @reg.setter
    def reg(self, v):
        self.__reg = v
        if self.__reg:
            self._len = 10
        else:
            self._len = 8

    @property
    def ret_offset(self):
        if not isinstance(self.parent, FunctionIR):
            return self.parent.offset + self._offset + 12
        else:
            return self._offset + 12

    @property
    def caller_index(self):
        return self.__caller_index

    @caller_index.setter
    def caller_index(self, v):
        assert isinstance(v, int)
        self.__caller_index = v

    @property
    def len(self):
        return self._len

    def asm(self):
        if self.reg:
            asmcode = "movw lr, #%d; movt lr, #%d" % (self.ret_offset, self.caller_index)
        else:
            asmcode = "mov r12, %s; movw lr, #%d; movt lr, #%d" % (self.reg, self.ret_offset, self.caller_index)
        code, count = IR._ks.asm(asmcode)
        self._code = bytearray(code)