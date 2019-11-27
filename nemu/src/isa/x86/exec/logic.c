#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test)
{
    rtl_and(&s0, &id_dest->val, &id_src->val);
    rtl_update_ZFSF(&s0, id_dest->width);
    rtl_set_OF(&ZERO);
    rtl_set_CF(&ZERO);

    print_asm_template2(test);
}

make_EHelper(and)
{
    rtl_sext(&s2, &id_src->val, id_src->width);
    rtl_and(&id_dest->val, &id_dest->val, &s2);
    rtl_update_ZFSF(&id_dest->val, id_dest->width);
    rtl_set_OF(&ZERO);
    rtl_set_CF(&ZERO);
    operand_write(id_dest, &id_dest->val);

    print_asm_template2(and);
}

make_EHelper (xor)
{
    rtl_sext(&s2, &id_src->val, id_src->width);
    rtl_xor(&id_dest->val, &id_dest->val, &s2);
    rtl_update_ZFSF(&id_dest->val, id_dest->width);
    rtl_set_OF(&ZERO);
    rtl_set_CF(&ZERO);
    operand_write(id_dest, &id_dest->val);

    print_asm_template2 (xor);
}

make_EHelper(or)
{
    rtl_sext(&s2, &id_src->val, id_src->width);
    rtl_or(&id_dest->val, &id_dest->val, &s2);
    rtl_update_ZFSF(&id_dest->val, id_dest->width);
    rtl_set_OF(&ZERO);
    rtl_set_CF(&ZERO);
    operand_write(id_dest, &id_dest->val);

    print_asm_template2(or);
}

make_EHelper(sar)
{
    rtl_sari(&s0, &id_dest->val, id_src->val);
    rtl_update_ZFSF(&s0, id_dest->width);
    operand_write(id_dest, &s0);
    // unnecessary to update CF and OF in NEMU

    print_asm_template2(sar);
}

make_EHelper(shl)
{
    rtl_shli(&s0, &id_dest->val, id_src->val);
    rtl_update_ZFSF(&s0, id_dest->width);
    operand_write(id_dest, &s0);
    // unnecessary to update CF and OF in NEMU

    print_asm_template2(shl);
}

make_EHelper(shr)
{
    rtl_shri(&s0, &id_dest->val, id_src->val);
    rtl_update_ZFSF(&s0, id_dest->width);
    operand_write(id_dest, &s0);
    // unnecessary to update CF and OF in NEMU

    print_asm_template2(shr);
}

make_EHelper(setcc)
{
    uint32_t cc = decinfo.opcode & 0xf;

    rtl_setcc(&s0, cc);
    operand_write(id_dest, &s0);

    print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not)
{
    rtl_not(&s0, &id_dest->val);
    operand_write(id_dest, &s0);

    print_asm_template1(not);
}

make_EHelper(rol)
{
    rtl_shri(&s0, &id_dest->val, 8 * id_dest->width - id_src->val);
    rtl_shl(&s1, &id_dest->val, &id_src->val);
    rtl_or(&s2, &s0, &s1);
    operand_write(id_dest, &s2);

    print_asm_template2(rol);
}