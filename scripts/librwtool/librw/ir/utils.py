from capstone.arm import *

_REG_STR = {
    ARM_REG_R0: "r0",
    ARM_REG_R1: "r1",
    ARM_REG_R2: "r2",
    ARM_REG_R3: "r3",
    ARM_REG_R4: "r4",
    ARM_REG_R5: "r5",
    ARM_REG_R6: "r6",
    ARM_REG_R7: "r7",
    ARM_REG_R8: "r8",
    ARM_REG_R9: "r9",
    ARM_REG_R10: "r10",
    ARM_REG_R11: "r11",
    ARM_REG_R12: "r12",
    ARM_REG_D0: "d0",
    ARM_REG_D1: "d1",
    ARM_REG_D2: "d2",
    ARM_REG_D3: "d3",
    ARM_REG_D4: "d4",
    ARM_REG_D5: "d5",
    ARM_REG_D6: "d6",
    ARM_REG_D7: "d7",
    ARM_REG_D8: "d8",
    ARM_REG_D9: "d9",
    ARM_REG_D10: "d10",
    ARM_REG_D11: "d11",
    ARM_REG_D12: "d12",
    ARM_REG_D13: "d13",
    ARM_REG_D14: "d14",
    ARM_REG_D15: "d15",
    ARM_REG_D16: "d16",
    ARM_REG_D17: "d17",
    ARM_REG_D18: "d18",
    ARM_REG_D19: "d19",
    ARM_REG_D20: "d20",
    ARM_REG_D21: "d21",
    ARM_REG_D22: "d22",
    ARM_REG_D23: "d23",
    ARM_REG_D24: "d24",
    ARM_REG_D25: "d25",
    ARM_REG_D26: "d26",
    ARM_REG_D27: "d27",
    ARM_REG_D28: "d28",
    ARM_REG_D29: "d29",
    ARM_REG_D30: "d30",
    ARM_REG_S0: "s0",
    ARM_REG_S1: "s1",
    ARM_REG_S2: "s2",
    ARM_REG_S3: "s3",
    ARM_REG_S4: "s4",
    ARM_REG_S5: "s5",
    ARM_REG_S6: "s6",
    ARM_REG_S7: "s7",
    ARM_REG_S8: "s8",
    ARM_REG_S9: "s9",
    ARM_REG_S10: "s10",
    ARM_REG_S11: "s11",
    ARM_REG_S12: "s12",
    ARM_REG_S13: "s13",
    ARM_REG_S14: "s14",
    ARM_REG_S15: "s15",
    ARM_REG_S16: "s16",
    ARM_REG_S17: "s17",
    ARM_REG_S18: "s18",
    ARM_REG_S19: "s19",
    ARM_REG_S20: "s20",
    ARM_REG_S21: "s21",
    ARM_REG_S22: "s22",
    ARM_REG_S23: "s23",
    ARM_REG_S24: "s24",
    ARM_REG_S25: "s25",
    ARM_REG_S26: "s26",
    ARM_REG_S27: "s27",
    ARM_REG_S28: "s28",
    ARM_REG_S29: "s29",
    ARM_REG_S30: "s30",
    ARM_REG_SP: "sp",
    ARM_REG_LR: "lr",
    ARM_REG_PC: "pc"
}

_COND_STR = {
    ARM_CC_EQ: "eq",
    ARM_CC_NE: "ne",
    ARM_CC_HS: "cs",
    ARM_CC_LO: "cc",
    ARM_CC_MI: "mi",
    ARM_CC_PL: "pl",
    ARM_CC_VS: "vl",
    ARM_CC_VC: "vc",
    ARM_CC_HI: "hi",
    ARM_CC_LS: "ls",
    ARM_CC_GE: "ge",
    ARM_CC_LT: "lt",
    ARM_CC_GT: "gt",
    ARM_CC_LE: "le",
    ARM_CC_AL: "",
}

_COND_ID = {
    "eq": ARM_CC_EQ,
    "ne": ARM_CC_NE,
    "hs": ARM_CC_HS,
    "cs": ARM_CC_HS,
    "lo": ARM_CC_LO,
    "cc": ARM_CC_LO,
    "mi": ARM_CC_MI,
    "pl": ARM_CC_PL,
    "vl": ARM_CC_VS,
    "vc": ARM_CC_VC,
    "hi": ARM_CC_HI,
    "ls": ARM_CC_LS,
    "ge": ARM_CC_GE,
    "lt": ARM_CC_LT,
    "gt": ARM_CC_GT,
    "le": ARM_CC_LE,
    "": ARM_CC_AL
}


def get_register_name(r_id):
    return _REG_STR[r_id]


def get_cond_name(c_id):
    return _COND_STR[c_id]


def get_cond_id(c_str):
    return _COND_ID[c_str]


def tohex(val, nbits):
    return (val + (1 << nbits)) % (1 << nbits)
