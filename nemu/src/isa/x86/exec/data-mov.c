#include "cpu/exec.h"

make_EHelper(mov)
{
    operand_write(id_dest, &id_src->val);
    print_asm_template2(mov);
}

make_EHelper(push)
{
    rtl_push(&id_dest->val);

    print_asm_template1(push);
}

make_EHelper(pop)
{
    rtl_pop(&id_dest->val);
    operand_write(id_dest, &id_dest->val);

    print_asm_template1(pop);
}

make_EHelper(pusha)
{
    TODO();

    print_asm("pusha");
}

make_EHelper(popa)
{
    TODO();

    print_asm("popa");
}

make_EHelper(leave)
{
    rtl_mv(&cpu.esp, &cpu.ebp);
    rtl_pop(&cpu.ebp);
    print_asm("leave");
}

make_EHelper(cltd)
{
    if (decinfo.isa.is_operand_size_16) {
        rtl_mv(&s0, &ZERO);
        if (reg_w(R_AX) < 0)
            s0 = 0xFFFF;
        rtl_sr(R_DX, &s0, 2);
    } else {
        rtl_mv(&s0, &ZERO);
        if (reg_l(R_EAX) < 0)
            s0 = 0xFFFFFFFF;
        rtl_sr(R_EDX, &s0, 4);
    }

    print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl)
{
    rtl_mv(&s0, &ZERO);
    if (decinfo.isa.is_operand_size_16) {
        rtl_lr(&s0, R_AL, 1);

        if (reg_b(R_AL) < 0)
            rtl_ori(&s1, &s0, 0xFF00);
        rtl_sr(R_AX, &s1, 2);
    } else {
        rtl_lr(&s0, R_AX, 2);
        printf("%x", s0);
        if (reg_w(R_AX) < 0)
            rtl_ori(&s1, &s0, 0xFFFF0000);
        rtl_sr(R_EAX, &s1, 4);
    }

    print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx)
{
    id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
    rtl_sext(&s0, &id_src->val, id_src->width);
    operand_write(id_dest, &s0);
    print_asm_template2(movsx);
}

make_EHelper(movzx)
{
    id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
    operand_write(id_dest, &id_src->val);
    print_asm_template2(movzx);
}

make_EHelper(lea)
{
    operand_write(id_dest, &id_src->addr);
    print_asm_template2(lea);
}
