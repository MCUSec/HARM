from capstone.arm import *
from capstone import *
from keystone import *

from .utils import get_cond_name
from copy import deepcopy


class IR:
    _ks = Ks(KS_ARCH_ARM, KS_MODE_THUMB)
    _md = Cs(CS_ARCH_ARM, CS_MODE_THUMB + CS_MODE_MCLASS)
    _md.skipdata_callback = lambda b, s, o, u: 4
    _md.skipdata = True
    _md.detail = True

    def __init__(self, offset=0, code=None, parent=None):
        self._parent = parent
        self._offset = offset
        if code:
            if isinstance(code, bytearray):
                self._code = code
            elif isinstance(code, bytes):
                self._code = bytearray(code)
            elif isinstance(code, str):
                binary, count = IR._ks.asm(code)
                self._code = bytearray(binary)
            else:
                raise IRInvalidCode
        else:
            self._code = bytearray()
        self._cond = ARM_CC_AL
        self._wide = False

    def __repr__(self):
        return "%s: IR %s" % (hex(self.addr), get_cond_name(self.cond))

    def __str__(self):
        if len(self._code) > 0:
            asmcode = ""
            for inst in self._md.disasm(bytes(self._code), self.addr):
                asmcode += "%s: %s\t%s\n" % (hex(inst.address), inst.mnemonic, inst.op_str)
            return asmcode.rstrip()
        else:
            return self.__repr__()

    @property
    def parent(self):
        return self._parent

    @parent.setter
    def parent(self, v):
        self._parent = v

    @property
    def offset(self):
        return self._offset

    @offset.setter
    def offset(self, v):
        assert isinstance(v, int)
        self._offset = v

    @property
    def addr(self):
        return self._parent.addr + self.offset if self._parent else self.offset

    @property
    def code(self):
        return deepcopy(self._code)

    @property
    def cond(self):
        return self._cond

    @cond.setter
    def cond(self, c):
        self._cond = c

    @property
    def len(self):
        return len(self._code)

    @property
    def wide(self):
        return len(self._code) > 2

    def asm(self):
        pass


class IRInvalidCode:
    pass
