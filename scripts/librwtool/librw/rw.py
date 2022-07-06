from capstone.arm import *

from librw.instrument.control_flow import *
from librw.instrument.interrupt import pendsv_hook_veneer_instrument, pendsv_instrument
from librw.instrument.misc import wfi_veneer_instrument
from librw.ir.adr import AddressToRegisterIR
from librw.ir.block import BlockIR
from librw.ir.branch import BranchIR
from librw.ir.firmware import FirmwareIR
from librw.ir.function import FunctionIR
from librw.ir.indirect_branch import IndirectBranchIR
from librw.ir.ir import IR
from librw.ir.it_block import ITBlockIR
from librw.ir.literal import LiteralPoolIR
from librw.ir.load_branch_address import LoadBranchAddressIR
from librw.ir.pop import PopIR
from librw.ir.ret import ReturnIR
from librw.ir.ret_encode import LoadReturnIndexIR, LoadFuncPtrIR
from librw.ir.table_branch import BranchTableIR, LoadBranchTableIR
from librw.ir.vector import VectorIR
from librw.ir.wfi import WfiIR


def do_instrument(src_ir, new_ir, pos='before'):
    if isinstance(src_ir.parent, ITBlockIR):
        new_ir.cond = src_ir.cond
        it_block = src_ir.parent
        if pos != 'replace':
            if it_block.size < 4:
                if src_ir.cond == it_block.first_cond:
                    it_block.insert_child(src_ir, new_ir, pos=pos)
                else:
                    it_block.insert_child(src_ir, new_ir, cond='e', pos=pos)
            else:
                new_it_block = it_block.split(it_block.child_index(src_ir))
                new_it_block.insert_child(src_ir, new_ir, pos=pos)
                assert isinstance(it_block.parent, FunctionIR)
                it_block.parent.insert_child(it_block, new_it_block, pos='after')
        else:
            it_block.insert_child(src_ir, new_ir, pos='replace')
    else:
        assert isinstance(src_ir.parent, FunctionIR)
        src_ir.parent.insert_child(src_ir, new_ir, pos=pos)


def reference_handoff(dst_ir, src_ir):
    if hasattr(src_ir, "ref_by"):
        for i in src_ir.ref_by:
            i.ref = dst_ir
        delattr(src_ir, "ref_by")


def fn_instrument(fn: FunctionIR, fw: FirmwareIR):
    report = {}
    jump_tables = []
    abs_literals = LiteralPoolIR()

    if fn.name[0:8] == "__Secure" or fn.name.find("__benchmark") == 0:
        first_ir = None
        for i in fn.child_iter():
            first_ir = i
            break
        do_instrument(first_ir, IR(code="orr lr, lr, #0x30000000"))
    else:
        need_instrument = lambda x: isinstance(x, BranchIR) and x.link or \
                                    isinstance(x, IndirectBranchIR) or \
                                    isinstance(x, PopIR) or \
                                    isinstance(x, ReturnIR) or \
                                    isinstance(x, LoadBranchAddressIR) or \
                                    isinstance(x, LiteralPoolIR) or \
                                    isinstance(x, AddressToRegisterIR) or \
                                    isinstance(x, LoadBranchAddressIR) or \
                                    isinstance(x, WfiIR)
        i_point = list()
        for i in fn.child_iter():
            if isinstance(i, BlockIR) and not (isinstance(i, BranchTableIR) or isinstance(i, LiteralPoolIR)):
                i_point += list(filter(lambda x: need_instrument(x), [ir for ir in i.child_iter()]))
            elif isinstance(i, BranchTableIR) and i.ref_by_load:
                jump_tables.append(i)
            elif isinstance(i, LiteralPoolIR):
                for literal in i.child_iter():
                    if literal.len == 8:
                        abs_literals.append_child(literal)
            else:
                if need_instrument(i):
                    i_point.append(i)

        objects = list(filter(lambda x: isinstance(x, VectorIR) or isinstance(x, FunctionIR),
                              [o for o in fw.child_iter()]))

        if len(i_point) > 0:
            for i in i_point:
                if isinstance(i, BranchIR):
                    assert i.link
                    i.link = False
                    ir = LoadReturnIndexIR(objects.index(fn), parent=fn)
                    reference_handoff(ir, i)
                    do_instrument(i, ir)
                    if "direct_call" in report:
                        report["direct_call"] += ir.len
                    else:
                        report["direct_call"] = ir.len
                elif isinstance(i, IndirectBranchIR):
                    if i.link:
                        if str(i.reg) != 'r12':
                            ir = LoadFuncPtrIR(i.reg, parent=fn)
                            reference_handoff(ir, i)
                            do_instrument(i, ir)
                        ir = LoadReturnIndexIR(objects.index(fn), parent=fn)
                        if str(i.reg) == 'r12':
                            reference_handoff(ir, i)
                        do_instrument(i, ir)
                        ir = BranchIR(i.offset, parent=fn)
                        ir.len = 4
                        ir.ref = fw.indirect_call_veneer
                        do_instrument(i, ir, pos='replace')
                        if "indirect_call" in report:
                            report["indirect_call"] += ir.len - i.len
                        else:
                            report["indirect_call"] = ir.len - i.len
                    elif str(i.reg) == 'lr':
                        if not fn.isr:
                            ir = BranchIR(i.offset, parent=fn)
                            ir.len = 4
                            ir.ref = fw.return_veneer.child_at(1)
                            reference_handoff(ir, i)
                            do_instrument(i, ir, pos='replace')
                            if "return" in report:
                                report["return"] += ir.len - i.len
                            else:
                                report["return"] = ir.len - i.len
                    else:
                        if str(i.reg) != 'r12':
                            ir = LoadFuncPtrIR(i.reg, parent=fn)
                            reference_handoff(ir, i)
                            do_instrument(i, ir)
                            if "indirect_branch" in report:
                                report["indirect_branch"] += ir.len
                            else:
                                report["indirect_branch"] = ir.len
                        ir = BranchIR(i.offset, parent=fn)
                        ir.len = 4
                        ir.ref = fw.indirect_branch_veneer
                        if str(i.reg) == 'r12':
                            reference_handoff(ir, i)
                        do_instrument(i, ir, pos='replace')
                        if "indirect_branch" in report:
                            report["indirect_branch"] += ir.len - i.len
                        else:
                            report["indirect_branch"] = ir.len - i.len
                elif (isinstance(i, PopIR) or isinstance(i, ReturnIR)) and not fn.isr:
                    """
                    pop {pc} / ldr pc, [sp], #4
                    """
                    ir = BranchIR(0)
                    ir.len = 4
                    ir.ref = fw.return_veneer

                    if isinstance(i, PopIR):
                        i.remove_reg(ARM_REG_PC)
                        do_instrument(i, ir, pos='after')
                        if "return" in report:
                            report["return"] += ir.len
                        else:
                            report["return"] = ir.len
                    else:
                        reference_handoff(ir, i)
                        do_instrument(i, ir, pos='replace')
                        if "return" in report:
                            report["return"] += ir.len - i.len
                        else:
                            report["return"] = ir.len - i.len
                elif isinstance(i, AddressToRegisterIR):
                    ir = LoadBranchTableIR(reg=i.reg, ref=i.ref)
                    if isinstance(i.ref, BranchTableIR):
                        i.ref.ref_by_load = ir
                        do_instrument(i, ir, pos='replace')
                        do_instrument(ir, LoadBranchTableIR(top=True, reg=i.reg, ref=i.ref), pos='after')
                elif isinstance(i, LoadBranchAddressIR):
                    i.dest_reg = i.base_reg
                    ir = IR(code="add pc, %s" % str(i.dest_reg))
                    do_instrument(i, ir, pos='after')
                elif isinstance(i, WfiIR):

                    ir = BranchIR(0)
                    ir.len = 4
                    ir.ref = fw.wfi_veneer
                    do_instrument(i, ir, pos='replace')

                    ir2 = LoadReturnIndexIR(objects.index(fn), parent=fn)
                    do_instrument(ir, ir2)

                    do_instrument(ir2, IR(0, "mov r12, lr"))
                    do_instrument(ir, IR(0, "mov lr, r12"), pos='after')
                else:
                    pass

    fn.commit()
    return report, jump_tables, abs_literals


def fw_instrument(fw, cmse_fn, has_rtos):
    fn_objs = list(filter(lambda x: isinstance(x, FunctionIR), [o for o in fw.child_iter()]))

    # instrument indirect call veneer
    indirect_branch_veneer_instrument(fw, fn_objs[-1], cmse_fn)
    indirect_call_veneer_instrument(fw, fn_objs[-1], cmse_fn)
    return_veneer_instrument(fw, fn_objs[-1], cmse_fn)
    wfi_veneer_instrument(fw, fn_objs[-1], cmse_fn)

    report = {}
    if has_rtos:
        pendsv_hook_veneer_instrument(fw, fn_objs[-1], cmse_fn)

    fw.commit()

    for fn in fw.child_iter():
        if isinstance(fn, FunctionIR) and fn.name not in ("indirect_call_veneer", "return_veneer",
                                                          "indirect_branch_veneer",
                                                          "PendSV_Hook0_veneer", "PendSV_Hook1_veneer"):
            if has_rtos and fn.name == "PendSV_Handler":
                report["PendSV"] = pendsv_instrument(fw, fn)
            fn_report, jump_tables, abs_literals = fn_instrument(fn, fw)
            for k in fn_report:
                if k in report:
                    report[k] += fn_report[k]
                else:
                    report[k] = fn_report[k]

            for i in jump_tables:
                fw.append_child(i)

            if abs_literals.size > 0:
                fw.append_child(abs_literals)

    fw.commit()

    return report


if __name__ == "__main__":
    pass