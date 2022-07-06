from .block import BlockIR
from .ref import *


class LiteralPoolIR(BlockIR):
    pass


class LiteralIR(IR):
    def __init__(self, offset=0, value=0xFFFFFFFF, parent=None):
        super().__init__(offset, None, parent)
        assert isinstance(value, int)
        if value <= 0xFFFFFFFF:
            self._code = bytearray(value.to_bytes(4, byteorder='little'))
        else:
            self._code = bytearray(value.to_bytes(8, byteorder='little'))

    @property
    def value(self):
        return int.from_bytes(self._code, 'little')

    @value.setter
    def value(self, v):
        assert isinstance(v, int)
        if v <= 0xFFFFFFFF:
            self._code = bytearray(v.to_bytes(4, byteorder='little'))
        else:
            self._code = bytearray(v.to_bytes(8, byteorder='little'))

    def __str__(self):
        if not hasattr(self, "reloc"):
            if self.value <= 0xFFFFFFFF:
                return "%s: %08x .word 0x%08x" % (hex(self.addr), self.value, self.value)
            else:
                return "%s: %08x .word 0x%08x\n" \
                       "%s: %08x .word 0x%08x" % (hex(self.addr),
                                                  self.value & 0xFFFFFFFF, self.value & 0xFFFFFFFF,
                                                  hex(self.addr + 4), self.value >> 32, self.value >> 32)
        else:
            return "%s: %08x .word 0x%08x (RELOC: %s)" % (hex(self.addr), self.value, self.value, self.reloc["type"])