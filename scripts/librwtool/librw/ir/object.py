from .ir import IR


class ObjectIR(IR):
    def __init__(self, name, code, align=4, offset=0, parent=None):
        assert align in (1, 2, 4)
        super().__init__(offset, code, parent)
        self._name = name
        self.__align = align
        # if len(self._code) % 2 != 0:
        #     print("%s: %d" % (name, len(self._code)))
        #     self._code += b'\xff'

    def __str__(self):
        return "%s @ %s (size: %s)" % (self.name, hex(self.addr), self.len)

    @property
    def name(self):
        return self._name

    @property
    def code(self):
        return super().code

    @code.setter
    def code(self, v):
        self._code = bytearray(v)

    @property
    def align(self):
        return self.__align

    @align.setter
    def align(self, v):
        assert v in (1, 2, 4)
        self.__align = v