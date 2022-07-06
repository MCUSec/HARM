from .object import ObjectIR


class VectorIR(ObjectIR):
    def __init__(self, code, offset=0, parent=None):
        assert len(code) == 0x130
        super().__init__("VectorTable", code=code, offset=offset, parent=parent)

    @property
    def vector(self):
        v = list()
        offset = 0
        while offset < len(self.code):
            v.append(int.from_bytes(self.code[offset:offset + 4], byteorder='little'))
            offset += 4
        return v

    def output_yaml_desp(self, idx):
        desp = {
            "index":  idx,
            "name": self.name,
            "kind": "VectorTable",
            "address": self.addr,
            "size": self.len,
            "isr": 0,
            "reloc_items": list()
        }
        return desp


