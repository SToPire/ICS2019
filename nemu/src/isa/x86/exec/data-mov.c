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
    s0 = cpu.esp;
    rtl_push(&cpu.eax);
    rtl_push(&cpu.ecx);
    rtl_push(&cpu.edx);
    rtl_push(&cpu.ebx);
    rtl_push(&s0);
    rtl_push(&cpu.ebp);
    rtl_push(&cpu.esi);
    rtl_push(&cpu.edi);

    print_asm("pusha");
}

make_EHelper(popa)
{
    rtl_pop(&cpu.edi);
    rtl_pop(&cpu.esi);
    rtl_pop(&cpu.ebp);
    rtl_pop(&s0);
    rtl_pop(&cpu.ebx);
    rtl_pop(&cpu.edx);
    rtl_pop(&cpu.ecx);
    rtl_pop(&cpu.eax);

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
            rtl_ori(&s0, &s0, 0xFF00);
        rtl_sr(R_AX, &s0, 2);
    } else {
        rtl_lr(&s0, R_AX, 2);
        if (reg_w(R_AX) < 0)
            rtl_ori(&s0, &s0, 0xFFFF0000);
        rtl_sr(R_EAX, &s0, 4);
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

make_EHelper(movsb)
{
    rtl_lm(&s0, &cpu.esi, 1);
    rtl_sm(&cpu.edi, &s0, 1);
    ++cpu.esi;
    ++cpu.edi;

    print_asm_template2(movsb);
}
make_EHelper(movs)
{
    if (decinfo.isa.is_operand_size_16) {
        rtl_lm(&s0, &cpu.esi, 2);
        rtl_sm(&cpu.edi, &s0, 2);
        cpu.esi += 2;
        cpu.edi += 2;
    } else {
        rtl_lm(&s0, &cpu.esi, 4);
        rtl_sm(&cpu.edi, &s0, 4);
        cpu.esi += 4;
        cpu.edi += 4;
    }

    print_asm_template2(movs);
}