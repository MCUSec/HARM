from librw.ir.arm_reg import ArmReg
from librw.ir.block import BlockIR
from librw.ir.branch import BranchIR
from librw.ir.firmware import FirmwareIR
from librw.ir.function import FunctionIR
from librw.ir.ir import IR
from librw.ir.ldr import LoadLiteralIR
from librw.ir.literal import LiteralIR

from capstone.arm import *

def pendsv_hook_veneer_instrument(fw: FirmwareIR, src: FunctionIR, cmse_fn):
    fn = FunctionIR("PendSV_Hook0_veneer", 0)

    ir1 = BlockIR(0)
    ir1.append_child(LiteralIR(0, value=cmse_fn["HARM_NSC_"]))

    ir2 = LoadLiteralIR(0)
    ir2.reg = ArmReg(ARM_REG_PC)
    ir2.ref = ir1
    ir2.len = 4

    fn.append_child(ir2)  # 0: ldr pc, PendSV_Hook
    fn.append_child(ir1)  # 4: PendSV_Hook
    fw.insert_child(src, fn, pos='after')
    stretched_size = fn.layout_refresh()
    fw.stretch(fn, stretched_size)
    setattr(fw, "PendSV_Hook0_veneer", fn)
    return fn.len


def pendsv_instrument(fw: FirmwareIR, fn: FunctionIR):
    first_ir = None
    for i in fn.child_iter():
        if not first_ir:
            first_ir = i
            break

    ir = BranchIR(0)
    ir.ref = fw.PendSV_Hook0_veneer
    ir.link = True

    fn.insert_child(first_ir, IR(0, bytearray((b"\x70\x46"))))  # 0: mov r0, lr
    fn.insert_child(first_ir, ir)  # 2: bl PendSV_Hook0_veneer
    fn.insert_child(first_ir, IR(0, bytearray(b"\x86\x46")))  # 6: mov lr, r0

    return fn.layout_refresh()