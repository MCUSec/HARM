from .utils import *


class ArmReg:
    def __init__(self, rid):
        self.__id = rid

    def __str__(self):
        return get_register_name(self.__id)

    @property
    def id(self):
        return self.__id

    @id.setter
    def id(self, v):
        self.__id = v
