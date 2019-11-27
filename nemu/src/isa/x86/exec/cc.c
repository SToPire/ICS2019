#include "rtl/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // TODO: Query eflags to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O:*dest=(cpu.eflags.OF==1);break;
    case CC_B:*dest=(cpu.eflags.CF==1);break;
    case CC_E:*dest=(cpu.eflags.ZF==1);break;
    case CC_BE:*dest=((cpu.eflags.CF==1)||(cpu.eflags.ZF==1));break;
    case CC_S:*dest=(cpu.eflags.SF==1);break;
    case CC_L:*dest=(cpu.eflags.SF!=cpu.eflags.OF);break;
    case CC_LE:*dest=((cpu.eflags.SF!=cpu.eflags.OF)||(cpu.eflags.ZF==1));break;
    default: panic("should not reach here");break;
    case CC_P: panic("n86 does not have PF");break;
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
  assert(*dest == 0 || *dest == 1);
}
