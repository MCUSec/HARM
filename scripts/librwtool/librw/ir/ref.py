from .ir import *
from abc import abstractmethod


class RefIR(IR):
    def __init__(self, offset, parent=None):
        super().__init__(offset, None, parent)
        self._ref = None
        self._ref_addr = -1
        self._len = 2

    def __str__(self):
        return super().__str__()

    @property
    def ref(self):
        return self._ref

    @property
    def ref_addr(self):
        return self._ref_addr if not self._ref else self._ref.addr

    @ref_addr.setter
    def ref_addr(self, v):
        self._ref_addr = v

    @ref.setter
    def ref(self, v):
        assert isinstance(v, IR)
        self._ref = v
        if hasattr(v, "ref_by"):
            v.ref_by.append(self)
        else:
            setattr(v, "ref_by", [self])

    @property
    def len(self):
        return self._len

    @len.setter
    def len(self, v):
        assert isinstance(v, int) and v in (2, 4)
        self._len = v

    @property
    def wide(self):
        return self._len > 2

    @abstractmethod
    def __calc_disp(self):
        pass

    @abstractmethod
    def asm(self):
        pass

    @abstractmethod
    def reachable(self):
        pass


class RefError(Exception):
    pass

