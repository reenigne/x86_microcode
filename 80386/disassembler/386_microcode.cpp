#include "alfe/main.h"
#include <inttypes.h>

class EntryPoint
{
public:
    EntryPoint(int u, int a, int t, String s) : _u(u), _a(a), _t(t), _s(s) { }
    int _u;  // Extra bits from decxoder rom. bit 1 is set if the initial bus address is SS:eSP instead of the EA from the instruction
    int _a;  // micro-op address of this entry point in microcode ROM
    int _t;  // Number of cycles the manual says that this instruction takes (may not be accurate)
    String _s;  // Name of entry point (x86 instructions or subroutine that this part of the microcode implements)
};

EntryPoint entryPoints[] = {
    {0, 0x003, 2, "   MOV r,r"},
    {0, 0x005, 2, "   MOV r,i"},
    {0, 0x007, 2, "   MOVZX rv,segreg"},
    {0, 0x009, 2, " r MOV ES/SS/DS/FS/GS,rw"},
    {1, 0x00b, 5, "   MOV SS,mw"},
    {1, 0x00f, 5, "   MOV ES/DS/FS/GS,mw"},
    {1, 0x013, 2, "   MOV [i],A  MOV m,r"},
    {1, 0x015, 2, "   MOV m,i"},
    {1, 0x017, 2, "   MOV mw,segreg"},
    {1, 0x019, 4, "   MOV A,[i]  MOV r,m"},
    {0, 0x01d, 2, "   ADD/OR/ADC/SBB/AND/SUB/XOR r,r"},
    {0, 0x01f, 2, "   CMP r,r  TEST r,r"},  // TEST reg,reg has cycle count of 1?
    {0, 0x021, 2, "   INC/DEC/NOT/NEG r"},
    {0, 0x023, 2, "   ADD/OR/ADC/SBB/AND/SUB/XOR r,i"},
    {0, 0x025, 2, "   CMP/TEST A,i"},
    {1, 0x027, 6, "   ADD/OR/ADC/SBB/AND/SUB/XOR r,m"},
    {1, 0x02c, 6, "   CMP r,m"},
    {1, 0x031, 5, "   CMP/TEST m,i"},
    {1, 0x035, 5, "   CMP/TEST m,r"},
    {9, 0x039, 7, "   ADD/OR/ADC/SBB/AND/SUB/XOR m,i"},
    {0, 0x03d, 4, "   SETcond rb"},
    { 0, 0x041, 0, "SETCONDR_FALSE"},
    {1, 0x043, 5, "   SETcond mb"},
    { 0, 0x046, 0, "WRITE_RESULT  "},
    { 0, 0x048, 0, "SETCONDM_FALSE"},
    {9, 0x04a, 7, "   ADD/OR/ADC/SBB/AND/SUB/XOR m,r"},
    {9, 0x04e, 6, "   INC/DEC/NOT/NEG m"},
    {0, 0x051, 5, "   JeCXZ cb"},  // cycle count is 5/9+m
    {0, 0x057,11, "   LOOP cb"},  // cycle count is 11+m/?
    { 0, 0x05b, 0, "LOOP_DONE     "},
    { 0, 0x05d, 0, "LOOP_UNTAKEN  "},
    {0, 0x05f,11, "   LOOPE/LOOPNE cb"},  // cycle count is 11+m/?
    {0, 0x065, 3, "   Jcond cb/cv"},  // cycle count is 3/7+m
    { 0, 0x067, 0, "JMP_PREFINAL  "},
    { 0, 0x068, 0, "JMP_FINAL     "},
    { 0, 0x069, 0, "Jcond_DONE    "},
    {0, 0x06a, 7, "   JMP c"},  // cycle count is 7+m
    { 0, 0x06b, 0, "JMP_PREF      "},
    {0, 0x06d, 7, "   JMP rv"},  // cycle count is 7+m
    {1, 0x06f,10, "   JMP mv"},  // cycle count is 10+m
    {3, 0x072,10, "   RET/RET iw"},  // cycle count is 10+m
    {2, 0x075, 7, "   CALL cw"},  // cycle count is 7+m
    {2, 0x079, 7, "   CALL rv"}, // cycle count is 7+m
    {1, 0x07c,10, "   CALL mv"}, // cycle count is 10+m
    {1, 0x081, 5, "   PUSH mv"},
    {2, 0x086, 2, "   PUSH rv"},
    {0, 0x088,24, "   PUSHAd"},
    { 0, 0x08d, 0, "PUSHAd_LOOP   "},
    {3, 0x091,24, "   POPA"},
    { 0, 0x092, 0, "POPAd_LOOP    "},
    {3, 0x097,24, "   POPAD"},
    {2, 0x09b, 2, "   PUSH segreg"},
    {2, 0x09d, 2, "   PUSH i"},
    {3, 0x09f, 4, "   POP rv"},  // number of cycles doesn't seem to match
    {3, 0x0a2, 7, "   POP SS"},
    {3, 0x0a7, 7, "   POP ES/DS/FS/GS"},
    {3, 0x0ac, 5, "   POP mv"},
    {9, 0x0b1, 5, "   XCHG m,r"},
    {0, 0x0b6, 3, "   XCHG eAX,rv  XCHG r,r"},
    {1, 0x0b9, 2, "   LEA rv,m"},
    {1, 0x0bb, 7, "   LDS rv,m"},  // cycle count is 7/22
    {1, 0x0c2, 7, "   LES rv,m"},  // cycle count is 7/22
    {1, 0x0c9, 7, "   LSS rv,m"},  // cycle count is 7/22
    {1, 0x0d0, 7, "   LFS/LGS rv,m"},  // cycle count is 7/22
    {0, 0x0d7, 2, "   LAHF"},
    {0, 0x0d9, 3, "   SAHF"},
    {0, 0x0dc, 5, "   CLTS"},
    {0, 0x0e1, 0, "   SALC"},  // don't know cycle count
    { 0, 0x0e4, 0, "SALC_DONE     "},
    {0, 0x0e5, 9, "   RCL/RCR r,ib"},
    {0, 0x0e9, 9, "   RCL/RCR r,CL"},
    { 0, 0x0ed, 0, "RCLRCR_R_COMM "},   // common to register ib and CL forms of RCL/RCR
    { 0, 0x0f1, 0, "RCLRCR_R_LOOP "},
    {0, 0x0f9, 3, "   ROL/ROR/SHL/SHR/SAR r,ib"},
    {0, 0x0fc, 3, "   SHxD rv,rv,ib"},
    {0, 0x0ff, 3, "   ROL/ROR/SHL/SHR/SAR r,CL"},
    {0, 0x102, 3, "   SHxD rv,rv,CL"},
    {0, 0x105, 9, "   rot r,1"},
    {1, 0x108,10, "   RCL/RCR m,ib"},
    {1, 0x10c,10, "   RCL/RCR m,CL"},
    { 0, 0x110, 0, "RCLRCR_M_COMM "},   // common to memory ib and CL forms of RCL/RCR
    { 0, 0x115, 0, "RCLRCR_M_LOOP "},
    {1, 0x11e, 7, "   ROL/ROR/SHL/SHR/SAR m,ib"},
    { 0, 0x120, 0, "ROSHSA_M_COMM "},   // common to memory ib and CL forms of ROL/ROR/SHL/SHR/SAR
    {1, 0x124, 7, "   SHxD mv,rv,ib"},
    { 0, 0x126, 0, "SHxD_M_COMM   "},   // common to memory ib and CL forms of ROL/ROR/SHL/SHR/SAR
    {1, 0x12a,10, "   rot m,1"},
    {1, 0x12d, 7, "   ROL/ROR/SHL/SHR/SAR m,CL"},
    {1, 0x12f, 7, "   SHxD mv,rv,CL"},
    {0, 0x131, 3, "   BT rv,rv"},
    {1, 0x134,12, "   BT m,rv"},
    {0, 0x13f, 3, "   BT rv,ib"},
    {1, 0x142, 6, "   BT m,ib"},
    {0, 0x147, 6, "   BTS/BTR/BTC rv,rv"},
    {9, 0x14d,13, "   BTS/BTR/BTC m,rv"},
    {0, 0x15a, 6, "   BTS/BTR/BTC rv,ib"},
    {9, 0x160, 8, "   BTS/BTR/BTC m,ib"},
    {0, 0x168,10, "   BSF rv,rv"},  // 10+3n
    { 0, 0x169, 0, "BSF_COMMON    "},   // common to register and memory forms of BSF
    { 0, 0x171, 0, "BSF_DONE      " },
    { 0, 0x172, 0, "BSF_LOOP      "},
    {1, 0x177,10, "   BSF rv,mv"},  // 10+3n
    {0, 0x17a,10, "   BSR rv,rv"},  // 10+3n
    { 0, 0x17b, 0, "BSR_COMMON    "},   // common to register and memory forms of BSR
    { 0, 0x17f, 0, "BSR_LOOP      "},
    { 0, 0x183, 0, "BSR_DONE      " },
    {1, 0x184,10, "   BSR rv,mv"},  // 10+3n
    {0, 0x187, 4, "   AAA/AAS"},
    {0, 0x18b, 4, "   DAA/DAS"},
    {0, 0x18f,17, "   AAM ib"},
    {0, 0x199,19, "   AAD ib"},
    {0, 0x1a5, 9, "   MUL/IMUL r"},  // cycle count is 9-14/9-22/9-38
    { 0, 0x1a7, 0, "iMUL_COMMON   "},   // common to register and memory forms of MUL/IMUL rm
    {1, 0x1af,12, "   MUL/IMUL m"},  // cycle count is 12-17/12-25/12-41
    {0, 0x1b3, 9, "   IMUL rv,rv"},  // cycle count is 9-22/9-38
    { 0, 0x1b5, 0, "IMUL2_COMMON   "},   // common to register and memory forms of IMUL rv,rmv and IMUL rv,rmv,i
    {1, 0x1bd,12, "   IMUL rv,mv"},  // cycle count is 12-25/12-41
    {1, 0x1c1, 9, "   IMUL rv,mv,i"},  // cycle count is 9-22/9-38
    {0, 0x1c4,12, "   IMUL rv,rv,i"},  // cycle count is 12-25/12-41
    {0, 0x1c7,14, "   DIV r "},  // cycle count is 14/22/38
    { 0, 0x1ca, 0, "DIV_COMMON     " },   // common to register and memory forms of DIV
    {1, 0x1cf,17, "   DIV m"},  // cycle count is 17/25/41
    {0, 0x1d3,19, "   IDIV r"},  // cycle count is 19/27/43
    { 0, 0x1d8, 0, "IDIV_COMMON     " },   // common to register and memory forms of IDIV
    {1, 0x1e1,22, "   IDIV m"},  // cycle count is 22/30/46
    {0, 0x1e6, 2, "   CWD / CDQ"},
    {0, 0x1e8, 3, "   CBW / CWDE  MOVZX/MOVSX r,r  (16-bit?)"},
    {1, 0x1eb, 6, "   MOVZX/MOVSX r,m  (16-bit?)"},
    {0, 0x1f0, 3, "   CBW / CWDE  MOVZX/MOVSX r,r  (32-bit?)"},
    {1, 0x1f3, 6, "   MOVZX/MOVSX r,m  (32-bit?)"},
    {0, 0x1f8, 7, "   REP MOVS"},
    { 0, 0x1fd, 0, "REP_MOVS_NOT0 " },
    { 0, 0x201, 0, "REP_MOVS_LOOP " },
    { 0, 0x205, 0, "REP_MOVS_DONE " },
    { 0, 0x207, 0, "REP_MOVS_RPTI " },
    { 0, 0x208, 0, "RPTI          "},
    {0, 0x211, 7, "   MOVS"},
    {0, 0x218,12, "   REP CMPS"},
    { 0, 0x21c, 0, "REP_CMPS_DONE " },
    { 0, 0x21d, 0, "REP_CMPS_LOOP " },
    { 0, 0x220, 0, "REP_CMPS_NOT0 " },
    {0, 0x22b,10, "   CMPS"},
    {0, 0x235, 9, "   REP SCAS"},
    { 0, 0x239, 0, "REP_SCAS_DONE " },
    { 0, 0x23a, 0, "REP_SCAS_LOOP " },
    { 0, 0x23d, 0, "REP_SCAS_NOT0 " },
    {0, 0x247, 7, "   SCAS"},
    {0, 0x24e, 7, "   REP LODS"},
    { 0, 0x24f, 0, "REP_LODS_DONE " },
    { 0, 0x250, 0, "REP_LODS_LOOP " },
    {0, 0x25a, 5, "   LODS"},
    {0, 0x25f, 5, "   XLATB"},
    {0, 0x263, 6, "   REP STOS"},
    { 0, 0x264, 0, "REP_STOS_DONE " },
    { 0, 0x265, 0, "REP_STOS_LOOP " },
    {0, 0x26e, 4, "   STOS"},
    {0, 0x272, 6, " p IN A,ib"},  // cycle count is 6/26
    {0, 0x274,12, " r IN A,ib"},
    { 0, 0x276, 0, "IN_IMM_NOCHK  " },
    { 0, 0x278, 0, "IN_DONE       " },
    {0, 0x279, 7, " p IN A,DX"},  // cycle count is 7/27
    {0, 0x27a,13, " r IN A,DX"},
    { 0, 0x27c, 0, "IN_DX_NOCHK   " },
    {0, 0x27f, 9, " p INS"},  // cycle count is 9/29
    {0, 0x281,15, " r INS"},
    { 0, 0x284, 0, "INS_NOCHK     " },
    {0, 0x28a,11, " p REP INS"},
    {0, 0x28b,17, " r REP INS"},
    { 0, 0x28d, 0, "REP_INS_NOCHK " },
    { 0, 0x290, 0, "REP_INS_DONE  " },
    { 0, 0x291, 0, "REP_INS_LOOP  " },
    { 0, 0x293, 0, "REP_INS_NOT0  " },
    {0, 0x29b, 4, " p OUT ib,A"},  // cycle count is 4/24
    {0, 0x29d,10, " r OUT ib,A"},
    { 0, 0x29f, 0, "OUT_IMM_NOCHK " },
    {0, 0x2a1, 5, " p OUT DX,A"},  // cycle count is 5/25
    {0, 0x2a2,11, " r OUT DX,A"},
    { 0, 0x2a4, 0, "OUT_DX_NOCHK  " },
    {0, 0x2a7, 8, " p OUTS"},  // cycle count is 8/28
    {0, 0x2a9,14, " r OUTS"},
    { 0, 0x2ac, 0, "OUTS_NOCHK    " },
    {0, 0x2b1,10, " p REP OUTS"},
    {0, 0x2b2,16, " r REP OUTS"},
    { 0, 0x2b4, 0, "REP_OUTS_NOCHK" },
    { 0, 0x2b7, 0, "REP_OUTS_DONE " },
    { 0, 0x2b8, 0, "REP_OUTS_LOOP " },
    { 0, 0x2ba, 0, "REP_OUTS_NOT0 " },
    { 0, 0x2c1, 0, "PORTIO_PROTCHK" },
    { 0, 0x2c4, 0, "IO_PERM_BITMAP" },
    { 0, 0x2d3, 0, "PORTIO_16BIT  " },
    { 0, 0x2d4, 0, "PORTIO_ALLOWED" },
    {2, 0x2d6,17, "   CALL cp"},  // cycle count is 17+m/34+m
    {1, 0x2da,22, "   CALL mp"},  // cycle count is 22+m/38+m
    { 0, 0x2e2, 0, "CALL_FAR_RM   " },  // common to cp and mp far CALLs in real mode
    {0, 0x2e5,12, "   JMP cp"},  // cycle count is 12+m/27+m/45+m/TS/TS
    { 0, 0x2eb, 0, "JMP_FAR_RM    " },
    { 0, 0x2ed, 0, "JMP_FAR_RM_1  " },  // Jumps to COUNTR:TMPG ? TMPB is previous CS limit (set by 2fb)?
    { 0, 0x2f0, 0, "JMP_FAR_COMMON" },
    { 0, 0x2f1, 0, "JMP_FAR_DONE  " },
    {1, 0x2f5,43, "   JMP mp"},  // cycle count is 43+m/31+m/49+m/5+TS/5+TS
    {3, 0x2fc,18, "   RETF/RETF iw"},  // cycle count is 18+m/32+m/68
    {2, 0x304,10, "   ENTER (16-bit?)"},  // cycle count is 10 (0), 12 (1), 15+4*(n-1) (n)
    {2, 0x308,10, "   ENTER (32-bit?)"},  // cycle count is 10 (0), 12 (1), 15+4*(n-1) (n)
    { 0, 0x30c, 0, "ENTER_COMMON  " },
    { 0, 0x316, 0, "ENTER_LOOP    " },
    { 0, 0x31a, 0, "ENTER_LAST    " },
    { 0, 0x31c, 0, "ENTER_LASTW   " },
    { 0, 0x31d, 0, "ENTER_DONE    " },
    {0, 0x31f, 4, "   LEAVE (16-bit?)"},
    {0, 0x323, 4, "   LEAVE (32-bit?)"},
    {0, 0x327, 5, "   HLT"},
    { 0, 0x329, 0, "HLT_SHUTDOWN  " },
    {1, 0x32e,11, "   LIDT mw"},
    {1, 0x332,11, "   LGDT mw"},
    { 0, 0x336, 0, "LIDTGDT_COMMON" },
    {0, 0x33d, 9, "   SIDT mw"},  // Number of cycles doesn't seem to match?
    {0, 0x340, 9, "   SGDT mw"},  // Number of cycles doesn't seem to match?
    { 0, 0x343, 0, "SIDTGDT_COMMON" },
    {1, 0x349,13, "   LMSW mw"},
    {0, 0x34e,10, "   LMSW rw"},
    { 0, 0x351, 0, "LMSW_COMMON   " },
    {0, 0x358,10, "   SMSW rw"},
    {1, 0x35a, 3, "   SMSW mw"},
    {0, 0x35c,10, "   MOV CR0,rd"},
    { 0, 0x366, 0, "PAGING_RM     " },
    {0, 0x368, 4, "   MOV CR2,rd"},
    {0, 0x36c, 5, "   MOV CR3,rd"},
    {0, 0x371, 6, "   MOV rd,CR0"},
    { 0, 0x373, 0, "STORE_CR      " },  // Common code to "MOV rd,CRn"
    {0, 0x377, 6, "   MOV rd,CR2"},
    {0, 0x379, 6, "   MOV rd,CR3"},
    {0, 0x37b,16, "   MOV DR4/6,rd"},
    {0, 0x37f,16, "   MOV DR5/7,rd"},
    { 0, 0x383, 0, "LD_DR4567_DONE" },
    { 0, 0x385, 0, "SELECT_DR_TR  " },
    {0, 0x38a,22, "   MOV DR0-3,rd"},
    {0, 0x391,12, "   MOV TRn,rd"},
    { 0, 0x398, 0, "GENERAL_DETECT" },   // Do the breakpoint on TR/DR access if bit 13 of DR7 is set
    { 0, 0x3a2, 0, "GD_HIT        " },   // General detect condition has been triggered
    {0, 0x3a6,22, "   MOV rd,DR4/6"},
    {0, 0x3aa,22, "   MOV rd,DR5/7"},
    {0, 0x3ae,22, "   MOV rd,DR0-3"},
    {0, 0x3b6,12, "   MOV rd,TRn"},
    {0, 0x3bd, 6, "   WAIT"},
    { 0, 0x3be, 0, "WAIT_LOOP     " },
    { 0, 0x3c1, 0, "WAIT_IRQT     " },  // Continuation of WAIT loop after subroutine - check if IRQ is pending
    { 0, 0x3c5, 0, "WAIT_ERRT     " },  // Subroutine to handle && in error test
    { 0, 0x3c8, 0, "WAIT_DONE     " },
    { 0, 0x3cb, 0, "FPU_ERROR6    " },
    {0, 0x3cd, 0, "   FENI/FDISI/FCLEX/FINIT/FSETPM/FRSTPM r"},  // FPU flag instructions (no memory access) - these need to reset prefetching for some reason? Same reason that CLI/STI do?
    { 0, 0x3d1, 0, "FPU_FLAG_LOOP " },
    {0, 0x3d9, 0, "   FSTCW/FSTSW mw"},  // Number of cycles doesn't seem to match?
    {0, 0x3da, 0, "   FSTSW AX/FNSTDW/FNSTSG r"},
    { 0, 0x3db, 0, "FPU_MISC_WAIT " },
    { 0, 0x3e2, 0, "FPU_MISC_CORE " },
    { 0, 0x3e8, 0, "FPU_MISC_WRITE" },
    { 0, 0x3eb, 0, "FPU_ERROR5    " },  // Unreachable?
    {1, 0x3ed, 0, "   FSTENV m"},
    {1, 0x3f0, 0, "   FSAVE m"},
    { 0, 0x3f3, 0, "FSAVE_WAIT    " },  // Common entry point to FSTENV and FSAVE - wait for FPU to be ready
    { 0, 0x3fa, 0, "FSAVE_CORE    " },
    { 0, 0x42f, 0, "FSAVE_COR_WAIT" },
    { 0, 0x42f, 0, "FSAVE_PM      " },  // FSAVE protected mode helper
    { 0, 0x436, 0, "FSAVE_REGFILE " },  // FSAVE subroutine to save register file (i.e. the part that isn't done in FSTENV)
    { 0, 0x43c, 0, "FSAVE_LOOP    " },
    { 0, 0x442, 0, "FSAVE_DONEFILE" },
    { 0, 0x444, 0, "FSAVE_NOWRITE " },
    { 0, 0x449, 0, "FSAVE_387     " }, //pla4 ?????1????<3E> 0000 10" },
    { 0, 0x451, 0, "FSAVE_287     " }, //pla4 ?????0????<3E> 0000 10" },
    { 0, 0x451, 0, "FSAVE_DONE    " },
    {1, 0x459, 0, "   FLDENV m"},
    {1, 0x45c, 0, "   FRSTOR m"},
    { 0, 0x45f, 0, "FPU_ERROR4    " },
    { 0, 0x461, 0, "FRSTOR_WAIT   " },  // Common entry point to FSTENV and FSAVE - wait for FPU to be ready
    { 0, 0x466, 0, "FRSTOR_IRQT   " },
    { 0, 0x46a, 0, "FRSTOR_ERRT   " },
    { 0, 0x46e, 0, "FRSTOR_CORE   " },
    { 0, 0x47b, 0, "FRSTOR_CORWAIT" },
    { 0, 0x49b, 0, "FRSTOR_PM     " },  // FRSTOR protected mode helper
    { 0, 0x4a7, 0, "FRSTOR_REGFILE" },  // FRSTOR subroutine to save register file (i.e. the part that isn't done in FLDENV)
    { 0, 0x4ad, 0, "FRSTOR_LOOP   " },
    { 0, 0x4b3, 0, "FRSTOR_DONEFIL" },
    { 0, 0x4b5, 0, "FRSTOR_387    " }, //pla4 ?????1????<3F> 0000 10" },
    { 0, 0x4ba, 0, "FRSTOR_DONE   " },
    { 0, 0x4bb, 0, "FRSTOR_287    " }, //pla4 ?????0????<3F> 0000 10" },
    {0, 0x4c1, 0, "   FP instructions that don't access memory"},
    { 0, 0x4c2, 0, "FPU_REG_WAIT  " },
    { 0, 0x4c5, 0, "FPU_REG_IRQT  " },
    { 0, 0x4c9, 0, "FPU_REG_ERRT  " },
    { 0, 0x4cc, 0, "FPU_REG_CORE  " },
    { 0, 0x4ce, 0, "FPU_REG_CORWAI" },
    { 0, 0x4d1, 0, "FPU_ERROR3    " },
    {1, 0x4d3, 0, "   FBLD/FLD mt"},
    {1, 0x4d7, 0, "   FADD/FMUL/FCOM/FCOMP/FSUB/FSUBR/FDIV/FDIVR/FIADD/FIMUL/FICOM/FICOMP/FISUB/FISUBR/FIDIV/FIDIVR/FLD/FILD md"},
    {1, 0x4db, 0, "   FLD/FILD/FADD/FMUL/FCOM/FCOMP/FSUBR/FSUB/FDIVR/FDIV mq"},
    {1, 0x4df, 0, "   FLDCW mw"},
    {1, 0x4e0, 0, "   FIADD/FIMUL/FICOM/FICOMP/FISUB/FISUBR/FIDIV/FIDIVR/FILD mw"},
    { 0, 0x4e7, 0, "FPU_LD80_287  " },
    { 0, 0x4e8, 0, "FPU_LD3264_287" },
    { 0, 0x4ee, 0, "FPU_LD80_387  " },
    { 0, 0x4f0, 0, "FPU_LOAD_WAIT " },
    { 0, 0x4f4, 0, "FPU_LOAD_IRQT " },
    { 0, 0x4f8, 0, "FPU_LOAD_ERRT " },
    { 0, 0x4fc, 0, "FPU_LOAD_CORE " },
    { 0, 0x503, 0, "FPU_LOAD_CORWA" },
    { 0, 0x506, 0, "FPU_LOAD_LOOP " },
    { 0, 0x50c, 0, "FPU_LOAD_DONE " },
    { 0, 0x50e, 0, "FPU_LD80_DONE " },
    { 0, 0x514, 0, "FPU_ERROR2    " },
    { 0, 0x516, 0, "FLDCW         " },
    { 0, 0x518, 0, "FLDCW_COR_WAIT" },
    { 0, 0x51c, 0, "FPU_LD3264_387" },
    { 0, 0x51f, 0, "FPU_LD3264_WAI" },
    { 0, 0x523, 0, "FPU_LD3264_IRQ" },
    { 0, 0x527, 0, "FPU_LD3264_ERR" },
    { 0, 0x52a, 0, "FPU_LD3264_COR" },
    { 0, 0x531, 0, "FPU_LD32      " },
    { 0, 0x533, 0, "FPU_LD32_WAIT " },  // Why wait after the data has been transferred? CPU bug?
    { 0, 0x536, 0, "#NM           " }, //pla4 ??????1???<3B><38><39><3C><3D> ??????01??<3B><38><39><3C><3D> ??????1?1?<34> 0000 10" },  coprocessor not available exception
    {1, 0x538, 0, "   FBSTP/FSTP mt"},
    {1, 0x53c, 0, "   FST/FSTP/FIST/FISTP md"},
    {1, 0x540, 0, "   FST/FSTP/FISTP mq"},
    {1, 0x544, 0, "   FIST/FISTP mw"},
    { 0, 0x54b, 0, "FPU_ERROR     ", },
    { 0, 0x54d, 0, "FPU_ST80_287  " },
    { 0, 0x54e, 0, "FPU_ST3264_287" },
    { 0, 0x554, 0, "FPU_ST80_387  " },
    { 0, 0x555, 0, "FPU_ST3264_387" },
    { 0, 0x556, 0, "FPU_STORE_WAIT" },
    { 0, 0x55a, 0, "FPU_STORE_IRQT" },
    { 0, 0x55e, 0, "FPU_STORE_ERRT" },
    { 0, 0x562, 0, "FPU_STORE_CORE" },
    { 0, 0x568, 0, "FPU_STORE_CORW" },
    { 0, 0x56f, 0, "FPU_STORE_LOOP" },
    { 0, 0x575, 0, "FPU_STORE_DONE" },
    { 0, 0x578, 0, "FPU_STORE_287 " },
    { 0, 0x57d, 0, "FPU_STORE_ABRT" },
    {0, 0x580, 2, " p MOV ES/DS/FS/GS,rw"},
    {0, 0x585, 2, " p MOV SS,rw"},
    { 0, 0x58a, 0, "PM_LD_DSESFSGS" },  // only called in protected mode, called before load of DS/ES/FS/GS happens
    { 0, 0x58e, 0, "PM_LD_SS      " },
    { 0, 0x592, 0, "NULL_SELECTOR " },
    { 0, 0x595, 0, "PM_LES        " },
    { 0, 0x59a, 0, "PM_LSS        " },
    { 0, 0x59f, 0, "PM_LDS        " },
    { 0, 0x5a4, 0, "PM_LFS_LGS    " },
    { 0, 0x5ac, 0, "JUMP_FAR_PM   " },
    { 0, 0x5b3, 0, "JUMP_FAR_PM_GATE" },
    { 0, 0x5b8, 0, "CALL_FAR_PM_GATE" },
    { 0, 0x5bb, 0, "GENERAL_FAULTP" },
    { 0, 0x5bd, 0, "CALLGATE286   " },
    { 0, 0x5be, 0, "CALLGATE386   " },
    { 0, 0x5c4, 0, "LD_DES_SSFSGS " },
    { 0, 0x5c9, 0, "LD_DESCRIPTOR " },
    { 0, 0x5ce, 0, "LD_DESCRIPTOR2" },
    { 0, 0x5d3, 0, "PRESENT_TSS   " },  // Available (<1F>) or Busy (<1E>)
    { 0, 0x5d5, 0, "PROT_TESTS_PASSED" },
    { 0, 0x5d8, 0, "PROT_TESTS_CMN" },
    { 0, 0x5da, 0, "PROT_TESTS_PASSED2" },
    { 0, 0x5df, 0, "PROT_TESTS_P16" },
    { 0, 0x5e0, 0, "CALL_FAR_PM   " },
    { 0, 0x5ec, 0, "MORE_PRIV16   " },
    { 0, 0x5f0, 0, "GENERAL_FAUL2P" },
    { 0, 0x5f2, 0, "PUSH_V86_SREGS" },  // Push(GS) code in https://www.felixcloutier.com/x86/intn:into:int3:int1 ?
    { 0, 0x5fb, 0, "MORE_PRIVILEGE" },  // CALL MORE-PRIVILEGE case in PRM
    { 0, 0x602, 0, "FOUND_STACK   " }, // We have found the offset of the SS/eSP pair in the TSS
    { 0, 0x60b, 0, "MORE_PRIV2    " },
    { 0, 0x613, 0, "MORE_PRIV3    " },
    { 0, 0x615, 0, "MORE_PRIV_INT " },
    { 0, 0x619, 0, "COPY_PARAMS   " },
    { 0, 0x61e, 0, "COPY_PRMS_LOOP" },
    { 0, 0x622, 0, "COPY_PRMS_DONE" },
    { 0, 0x623, 0, "COPY_PRMS_SKIP" },
    { 0, 0x62b, 0, "NO_ERROR_CODE " },
    { 0, 0x633, 0, "MORE_PRIV_DONE" },
    { 0, 0x63b, 0, "ZERO_V86_SREGS" },  // GS := 0 code in https://www.felixcloutier.com/x86/intn:into:int3:int1 ?
    { 0, 0x63f, 0, "IRETd_V86     " },  // Returns to? from? v86 mode? real mode?
    { 0, 0x657, 0, "IRETd_V86_LOOP" },
    { 0, 0x65c, 0, "TASK_RETURN   " },  // TASK-RETURN described in https ://www.felixcloutier.com/x86/iret:iretd:iretq ?
    { 0, 0x671, 0, "LOAD_TASK_16B " },
    {3, 0x673,38, " p IRETd"},  // cycle count is 38/82/TS/60
    { 0, 0x67f, 0, "RETF_PM       " },
    { 0, 0x686, 0, "RETF_OUTER_LEV" },
    { 0, 0x699, 0, "RETF_OL_ES    " },
    { 0, 0x69d, 0, "RETF_OL_DS    " },
    { 0, 0x6a1, 0, "RETF_OL_FS    " },
    { 0, 0x6a5, 0, "RETF_OL_GS    " },
    { 0, 0x6a8, 0, "ZERO_SLCTR_AR " },
    {1, 0x6aa,21, " p ARPL mw,rw"},
    { 0, 0x6b3, 0, "ARPL_FAILED   " },
    {0, 0x6b6,20, " p ARPL rw,rw"},
    {1, 0x6be,27, " p LTR mw"},
    {0, 0x6c3,23, " p LTR rw"},
    { 0, 0x6c5, 0, "LTR_COMMON    " },
    {1, 0x6cd, 2, " p STR mw"},
    {0, 0x6cf, 2, " p STR rw"},
    {1, 0x6d1,24, " p LLDT mw"},
    {0, 0x6d6,20, " p LLDT rw"},
    { 0, 0x6c8, 0, "LLDT_COMMON   " },
    { 0, 0x6dd, 0, "LLDT_TEST_PASS" },
    {1, 0x6e0, 2, " p SLDT mw"},
    {0, 0x6e2, 2, " p SLDT rw"},
    {1, 0x6e4,16, " p LAR rv,mv"},
    {0, 0x6e7,15, " p LAR rv,rv"},
    {1, 0x6ea,21, " p LSL rv,mv"},  // cycle count is 21/26
    {0, 0x6ec,20, " p LSL rv,rv"},  // cycle count is 20/25
    { 0, 0x6ee, 0, "LSL_HELPER    "},
    { 0, 0x6f5, 0, "LSL_GRANULARITY_COARSE" },
    { 0, 0x6fd, 0, "LSL_GRANULARITY_FINE" },
    {1, 0x700,11, " p VERR mw"},
    {0, 0x703,10, " p VERR rw"},
    {1, 0x706,16, " p VERW mw"},
    {0, 0x709,15, " p VERW rw"},
    { 0, 0x70c, 0, "LAR_LSL_VERRWM" },
    { 0, 0x70e, 0, "LAR_LSL_VERRWR" },
    { 0, 0x70f, 0, "LAR_LSL_VERRW " },
    { 0, 0x71a, 0, "LAR_VERRW_SUCCEEDED" }, //pla4 0????01011<30> 0????01100<30> 0????1011?<30><32> 0????10?0?<32> 0????10?1?<33> 0????1101?<30> 0????1?000<30> 0????1?01?<32> 0?????001?<30> 0?????010?<30> 0??????001<30> ?????1111?<32> ?????111??<30> 0000 10" },
    { 0, 0x71c, 0, "TASKGATE      " },
    { 0, 0x71f, 0, "AVAIL_TSS     " },
    { 0, 0x724, 0, "SAVE_TASK     " },
    { 0, 0x72b, 0, "SAVE_TASK_16  " },
    { 0, 0x72e, 0, "SAVE_TASK_COMM" },
    { 0, 0x737, 0, "SAVE_TASK_LOOP" },
    { 0, 0x73f, 0, "CALL_SAVE_TASK" },
    { 0, 0x743, 0, "AVAIL_TSS_PR  " },
    { 0, 0x749, 0, "SWITCH_TASK   " },
    { 0, 0x750, 0, "LOAD_TASK     " },
    { 0, 0x758, 0, "TR_BACKLINK_OK" },
    { 0, 0x75a, 0, "LOAD_TASK_32  " },
    { 0, 0x764, 0, "LOAD_TASK_16  " },
    { 0, 0x765, 0, "LOAD_TASK_16A " },
    { 0, 0x765, 0, "LOAD_TASK_COMM" },  // Common to both 16-bit and 32-bit LOAD_TASK
    { 0, 0x773, 0, "LOAD_TASK_LOOP" },
    { 0, 0x787, 0, "LOAD_TASK_NOPG" },
    { 0, 0x78c, 0, "NOT_NESTED_TSK" },
    { 0, 0x792, 0, "SET_TSKSWTCHED" },
    { 0, 0x795, 0, "WRITE_BACK_CR0" },
    { 0, 0x798, 0, "TASK_FINAL_V86" },
    { 0, 0x798, 0, "TASK_FV86_LOOP" },
    { 0, 0x7ab, 0, "TASK_FINAL_PM " },
    { 0, 0x7d0, 0, "HANDLE_TS_BP  " },  // Last phase of task switching - handle breakpoints
    { 0, 0x7d4, 0, "ADJ_STACK_DONE" },
    { 0, 0x7d7, 0, "TSKF_NO_ERRCOD" },
    { 0, 0x7e3, 0, "ADJ_STACK_16  " },
    { 0, 0x7e5, 0, "NULL_SELECTOR2" },
    { 0, 0x7e8, 0, "AVAIL_TSS_NP  " },
    { 0, 0x7eb, 0, "NO_DEBUG_BP   " },
    { 0, 0x7ec, 0, "CLI_STI       " },
    {0, 0x7ef, 2, "   CLC/STC/CLD/STD/CMC"},
    {2, 0x7f2, 4, " p PUSHFd"},
    {0, 0x7f7, 3, "   CLI/STI"},  // cycle count is 2 for STI?
    {2, 0x7f9, 4, " r PUSHFd"},
    {3, 0x7fb, 5, " r POPFd"},
    {0, 0x7fd,37, " r INT ib"},
    {3, 0x7ff,22, " r IRETd"},  // cycle count is 22/38
    { 0, 0x801, 0, "OVERLONG_INST "}, //small pla 010100 01 ? 0011011" },
    {3, 0x803, 5, " p POPFd"},
    {0, 0x808,59, " p INT ib"},  // cycle count is 59/99/119/TS
    { 0, 0x80a, 0, "IRET_REAL_MODE" },
    {0, 0x816,33, "   INT 3"},  // cycle count is 33/59/99/119/TS
    {1, 0x818,10, "   BOUND rv,m2v"},   // cycle count in no-jump case
    { 0, 0x822, 0, "#BR           " }, //small pla ?????? 11 ? 0011010" },   bound range exceeded
    { 0, 0x824, 0, "DIVIDE_ERROR  " }, //small pla ?????? 11 ? 0011010" },
    {0, 0x827, 3, "   INTO"},  // cycle count is 35/3/59/99/119/TS
    { 0, 0x82a, 0, "NO_OVERFLOW   " },
    { 0, 0x82b, 0, "#UD           " },  // invalid opcode
    { 0, 0x82d, 0, "HARDWARE_IRQ  " }, //small pla 11010? 01 ? 0011011" },
    { 0, 0x836, 0, "NMI           " }, //small pla ?0010? 01 ? 0011011" },
    { 0, 0x839, 0, "TRIPLE_FAULT  " }, //small pla ?????1 00 ? 0000000" },
    { 0, 0x83a, 0, "TRIPLE_FT_WAIT" },
    { 0, 0x83f, 0, "#DF           " }, //small pla ?????? 00 1 0100010" },     double fault
    {0, 0x844, 0,  "NO_PRIVILEGE  " },
    {0, 0x849, 0, "   unused?"},
    { 0, 0x857, 0, "ACCESS_VIOLATI" }, //small pla ?????? 00 ? 0010110" },
    { 0, 0x85b, 0, "#GP(0)        " },
    { 0, 0x85d, 0, "#GP/#TS(I0,E0)" },
    { 0, 0x85e, 0, "#GP/#TS(SIGMA)" }, //pla4 ?????0011?<24> ?????01001<24> 0000 10" },
    { 0, 0x860, 0, "#GP(SIGMA)    " },
    { 0, 0x862, 0, "#SS(0)        " },
    { 0, 0x863, 0, "#SS(SIGMA)    " },
    { 0, 0x865, 0, "#GP(I1,E0)    " },  // #GP((TMP_TR & -4) | 2)       general fault with EXT = 0, I = 1  => fault with gate descriptor in the IDT
    { 0, 0x866, 0, "#GP(SIGMA | 2)" },  //                      I = 1
    { 0, 0x868, 0, "#TS(SIGMA)    " },  // #TS(SIGMA)
    { 0, 0x86a, 0, "#SS(I0,E0)    " },  // #SS(TMP_TR & -4)
    { 0, 0x86d, 0, "FAULT_SUPPRESS" },
    { 0, 0x86e, 0, "LAR_LSL_VERRW_NULL_SELECTOR" },
    { 0, 0x870, 0, "#NP(I0,E0)    " },  // #NP(TMP_TR & -4)
    { 0, 0x871, 0, "#NP(I1,E0)    " },  // #NP((TMP_TR & -4)|2)
    { 0, 0x873, 0, "NP_COMMON     " },  // Common to both #NP with I=0 and I=1
    { 0, 0x874, 0, "FAULT_ERR_CODE" },
    { 0, 0x87b, 0, "EXT_BIT_DONE  " },
    { 0, 0x882, 0, "STRING_CORRECT" },
    { 0, 0x889, 0, "STRCORR_DOWN  " },
    { 0, 0x88a, 0, "STRCORR_LOOP  " },
    { 0, 0x890, 0, "FAULT         " },
    { 0, 0x893, 0, "FLAGS_OK      " },
    { 0, 0x89a, 0, "RESUME_FLAG_OK" },
    { 0, 0x89e, 0, "INTERRUPT_X   " },
    { 0, 0x89f, 0, "INTERRUPT     " }, // Software or hardware interrupt (vector entries are 4 byte far pointers)
    { 0, 0x8b0, 0, "INTERRUPT_PM  " }, // Software or hardware interrupt in protected or v86 mode (vector entries are 8 byte gate descriptors)
    { 0, 0x8ba, 0, "INTERRUPT_COMM" },
    { 0, 0x8bf, 0, "INTERRUPT_SW  " },
    { 0, 0x8c2, 0, "INTGATE286    " },
    { 0, 0x8c3, 0, "TRAPGATE286   " },
    { 0, 0x8cb, 0, "INTGATE386    " },
    { 0, 0x8cc, 0, "TRAPGATE386   " },
    { 0, 0x8d5, 0, "TRAP_INT_GATE " }, // common to 286/386 trap/interrupt gates
    { 0, 0x8d5, 0, "PUSH_INT_FRAME" },  // pushes flags, return address and error code
    { 0, 0x8e3, 0, "TRAP_INT_DONE " }, // end of trap/interrupt gate code
    { 0, 0x8e5, 0, "PUSH_ERRORCODE" },  // pushes the error code onto the stack for faults
    { 0, 0x8e9, 0, "PAGE_FAULT    " }, //small pla ?????? 00 0 1011010" },
    { 0, 0x8ea, 0, "PAGE_FAULTLOOP" },
    {0, 0x8f6, 0, "   LOADALL386"},  // don't know cycle count
    { 0, 0x902, 0, "LOADALL_LOOP1 " },
    { 0, 0x90b, 0, "LOADALL_LOOP2 " },
    { 0, 0x917, 0, "LOADALL_LOOP3 " },
    { 0, 0x91c, 0, "LOADALL_LOOP4 " },
    { 0, 0x932, 0, "LOADALL_FINAL " },
    { 0, 0x939, 0, "LOADALL_PAGING" },
    {0, 0x93c, 0, "   ICEBP"},  // don't know cycle count
    { 0, 0x93f, 0, "SINGLE_STEP   " }, //small pla ???0?? 01 ? 0011011" },  single stepping via trap flag
    { 0, 0x941, 0, "BREAKPOINT    " }, //small pla ??11?? 01 ? 0011011" },  comes in via small pla when DR0-DR3 are hit?  Also jumped to from microcode for task switch breakpoint
    { 0, 0x943, 0, "BREAKPOINT_CMN" }, // Breakpoint code common to software breakpoints (ICEBP) and hardware/signal breakpoints (like debug register condition getting hit)
    { 0, 0x94f, 0, "ICE_SINGLESTEP" }, // SINGLE_STEP handled by ICE
    { 0, 0x953, 0, "ICE_PIN       " }, //small pla ??011? 01 ? 0011011" },            Enter ICE mode via ICE# pin like SMI# pin?
    { 0, 0x95a, 0, "ICE_PIN_LOOP  " },
    { 0, 0x95e, 0, "STOREALL16    " },
    { 0, 0x961, 0, "STOREALL      " },
    { 0, 0x95e, 0, "STOREALL_COMM " },
    { 0, 0x970, 0, "STOREALL_LOOP1" },
    { 0, 0x980, 0, "STOREALL_LOOP2" },
    { 0, 0x981, 0, "STOREALL_LOOPS" },
    { 0, 0x989, 0, "STOREALL_LOOP3" },
    { 0, 0x993, 0, "STOREALL_LOOP4" },
    { 0, 0x99a, 0, "STOREALL_L4_ST" },
    { 0, 0x9a6, 0, "BOOTUP        " },
    { 0, 0x9a7, 0, "BOOTUP_LOOP1  " },
    { 0, 0x9ab, 0, "BOOTUP_LOOP2  " },
    { 0, 0x9af, 0, "BOOTUP_LOOP3  " },
    { 0, 0x9bc, 0, "BOOTUP_JUMP   " }, // Jump to initial boot CS:IP (BASE=0xffff0000 LIMIT=0xffff):0x0000fff0 aka linear address 0xfffffff0
    {-1, -1, -1, ""}};

class Bitset
{
public:
    Word _mask;
    Word _bits;
    Word _output;
};

class Bitset1
{
public:
    DWord _mask;
    DWord _bits;
    Word _output;
};

class Bitset4
{
public:
    Word _mask;
    Word _bits;
    DWord _output;
};

class Program : public ProgramBase
{
public:
    void dorom1()
    {
        String input1 = File("rom1_input_new.txt", true).contents();
        String output1 = File("rom1_output_new.txt", true).contents();
        Array<DWord> mask1(175);
        Array<DWord> bits1(175);
        Array<Word> outputs1(175);
        Array<Word> ored1(524288);
        for (int r = 0; r < 175; ++r) {
            int m = 0;
            int b = 0;
            for (int i = 0; i < 19; ++i) {
                if (input1[r*21 + i] != '.')
                    m += 1 << i;
                if (input1[r*21 + i] == '1')
                    b += 1 << i;
            }
            mask1[r] = m;
            bits1[r] = b;
            int o = 0;
            for (int i = 0; i < 12; ++i) {
                if (output1[r*14 + i] == '1')
                    o += 1 << (11 - i);
            }
            outputs1[r] = o;
        }
        for (int o = 0; o < 524288; ++o) {
            int v = 0;
            for (int row = 0; row < 175; ++row) {
                if ((o & mask1[row]) == bits1[row])
                    v |= outputs1[row];
            }
            ored1[o] = v;
        }

        for (int n = 0; n < 175; ++n) {
            int o;
            for (o = 0; o < 524288; ++o) {
                int v = 0;
                for (int row = 0; row < 175; ++row) {
                    if (row == n)
                        continue;
                    if ((o & mask1[row]) == bits1[row])
                        v |= outputs1[row];
                }
                if (v != ored1[o])
                    break;
            }
            if (o == 524288)
                console.write(decimal(n) + " was redundant.\n");
        }

        Array<Bitset1> bitsets1(524288);
        for (int x = 0; x < 0xfff; ++x) {
            int nbitsets1 = 0;
            for (int o = 0; o < 524288; ++o) {
                if (ored1[o] == x) {
                    bitsets1[nbitsets1]._mask = 0x7ffff;
                    bitsets1[nbitsets1]._bits = o;
                    bitsets1[nbitsets1]._output = ored1[o];
                    ++nbitsets1;
                }
            }
            while (true) {
                bool found = false;
                //console.write(decimal(nbitsets1) + "\n");
                for (int o = nbitsets1 - 1; o >= 1; --o) {
                    for (int o1 = o - 1; o1 >= 0; --o1) {
                        if (bitsets1[o]._mask != bitsets1[o1]._mask || bitsets1[o]._output != bitsets1[o1]._output)
                            continue;
                        DWord b = bitsets1[o]._bits ^ bitsets1[o1]._bits;
                        if ((b & (b-1)) == 0) {
                            found = true;
                            bitsets1[o1]._mask &= ~b;
                            bitsets1[o1]._bits &= ~b;
                            for (int o2 = o; o2 < nbitsets1 - 1; ++o2) {
                                bitsets1[o2]._mask = bitsets1[o2 + 1]._mask;
                                bitsets1[o2]._bits = bitsets1[o2 + 1]._bits;
                                bitsets1[o2]._output = bitsets1[o2 + 1]._output;
                            }
                            --nbitsets1;
                            if (o == nbitsets1)
                                break;
                        }
                    }
                }

                if (!found)
                    break;
            }

            for (int o = 0; o < nbitsets1; ++o) {
                for (int b = 0; b < 19; ++b) {
                    if ((bitsets1[o]._mask & (1 << b)) == 0)
                        console.write("?");
                    else
                        if ((bitsets1[o]._bits & (1 << b)) == 0)
                            console.write("0");
                        else
                            console.write("1");
                }
                console.write(" ");
                for (int b = 11; b >= 0; --b) {
                    if ((bitsets1[o]._output & (1 << b)) == 0)
                        console.write("0");
                    else
                        console.write("1");
                }
                console.write("  ");
                console.write("\n");
            }
        }
    }
    void dopla4()
    {
        String input4 = File("pla4_input_new.txt", true).contents();
        String output4 = File("pla4_output_new.txt", true).contents();
        Array<Word> mask4(160);
        Array<Word> bits4(160);
        Array<DWord> outputs4(160);
        Array<DWord> ored4(65536);
        for (int r = 0; r < 160; ++r) {
            int m = 0;
            int b = 0;
            for (int i = 0; i < 16; ++i) {
                if (input4[r*18 + i] != '.')
                    m += 1 << i;
                if (input4[r*18 + i] == '1')
                    b += 1 << i;
            }
            mask4[r] = m;
            bits4[r] = b;
            int o = 0;
            for (int i = 0; i < 18; ++i) {
                if (output4[r*20 + i] == '1')
                    o += 1 << (17 - i);
            }
            outputs4[r] = o;
        }
        for (int o = 0; o < 65536; ++o) {
            int v = 0;
            for (int row = 0; row < 160; ++row) {
                if ((o & mask4[row]) == bits4[row])
                    v |= outputs4[row];
            }
            ored4[o] = v;
        }

        //for (int n = 0; n < 160; ++n) {
        //    int o;
        //    for (o = 0; o < 65536; ++o) {
        //        int v = 0;
        //        for (int row = 0; row < 160; ++row) {
        //            if (row == n)
        //                continue;
        //            if ((o & mask4[row]) == bits4[row])
        //                v |= outputs4[row];
        //        }
        //        if (v != ored4[o])
        //            break;
        //    }
        //    if (o == 65536)
        //        console.write(decimal(n) + " was redundant.\n");
        //}

        Array<Bitset4> bitsets4(65536);
        for (int x = 0; x < 262144; ++x) {
            int nbitsets4 = 0;
            for (int o = 0; o < 65536; ++o) {
                if (ored4[o] == x) {
                    bitsets4[nbitsets4]._mask = 0xffff;
                    bitsets4[nbitsets4]._bits = o;
                    bitsets4[nbitsets4]._output = ored4[o];
                    ++nbitsets4;
                }
            }
            while (true) {
                bool found = false;
                //console.write(decimal(nbitsets1) + "\n");
                for (int o = nbitsets4 - 1; o >= 1; --o) {
                    for (int o1 = o - 1; o1 >= 0; --o1) {
                        if (bitsets4[o]._mask != bitsets4[o1]._mask || bitsets4[o]._output != bitsets4[o1]._output)
                            continue;
                        DWord b = bitsets4[o]._bits ^ bitsets4[o1]._bits;
                        if ((b & (b-1)) == 0) {
                            found = true;
                            bitsets4[o1]._mask &= ~b;
                            bitsets4[o1]._bits &= ~b;
                            for (int o2 = o; o2 < nbitsets4 - 1; ++o2) {
                                bitsets4[o2]._mask = bitsets4[o2 + 1]._mask;
                                bitsets4[o2]._bits = bitsets4[o2 + 1]._bits;
                                bitsets4[o2]._output = bitsets4[o2 + 1]._output;
                            }
                            --nbitsets4;
                            if (o == nbitsets4)
                                break;
                        }
                    }
                }

                if (!found)
                    break;
            }

            for (int o = 0; o < nbitsets4; ++o) {
                for (int b = 0; b < 16; ++b) {
                    if ((bitsets4[o]._mask & (1 << b)) == 0)
                        console.write("?");
                    else
                        if ((bitsets4[o]._bits & (1 << b)) == 0)
                            console.write("0");
                        else
                            console.write("1");
                }
                console.write(" ");
                for (int b = 17; b >= 0; --b) {
                    if ((bitsets4[o]._output & (1 << b)) == 0)
                        console.write("0");
                    else
                        console.write("1");
                }
                console.write("\n");
            }
        }
    }

    String printuop(uint64_t u)
    {
        static const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789&";
        String line;
        if (u == 0)
            return "=====================================";
        for (int i = 0; i < 37; ++i) {
            if ((u & (1LL << i)) != 0) {
                line += codePoint(chars[i]);
            }
            else
                line += " ";
        }
        return line;
    }

    void run() {
        dorom1();
#if 0
        dopla4();
#endif

        String inputL = File("bits_l.py", true).contents();
        String inputR = File("bits_r.py", true).contents();
        String outputL = File("bits_addr_l.py", true).contents();
        String outputR = File("bits_addr_r.py", true).contents();
        Array<Word> mask(368);
        Array<Word> bits(368);
        Array<Word> outputs(368);
        Array<Word> ored(8192);
        //Array<Word> oredL(8192);
        //Array<Word> oredR(8192);
        for (int row = 0; row < 368; ++row) {
            mask[row] = 0;
            bits[row] = 0;
            outputs[row] = 0;
        }
        for (int row = 0; row < 368; ++row) {
            String input = inputL;
            String output = outputL;
            int r = row;
            if (row >= 184) {
                input = inputR;
                output = outputR;
                r -= 184;
            }
            int m = 0;
            int b = 0;
            for (int i = 0; i < 13; ++i) {
                if (input[r*58 + i*4 + 10] == '1' || input[r*58 + i*4 + 12] == '1')
                    m += 1 << (12 - i);
                if (input[r*58 + i*4 + 12] == '1')
                    b += 1 << (12 - i);
            }
            mask[row] = m;
            bits[row] = b;
            int o = 0;
            for (int i = 0; i < 16; ++i) {
                if (output[r*38 + i*2 + 10] == '1')
                    o += 1 << i;
            }
            outputs[row] = o;
        }
        for (int o = 0; o < 8192; ++o) {
            int v = 0;
            for (int row = 0; row < 368; ++row) {
                if ((o & mask[row]) == bits[row])
                    v |= outputs[row];
            }
            ored[o] = v;
            if ((v & 0xfff) >= 2560) {
                console.write("ored[" + hex(o, 4, false) + "] = " + hex(v, 4, false) + "\n");
            }
            //int vL = 0;
            //for (int row = 0; row < 184; ++row) {
            //    if ((o & mask[row]) == bits[row])
            //        vL |= outputs[row];
            //}
            //oredL[o] = vL;
            //int vR = 0;
            //for (int row = 184; row < 368; ++row) {
            //    if ((o & mask[row]) == bits[row])
            //        vR |= outputs[row];
            //}
            //oredR[o] = vR;
            //ored[o] = vL | vR;
        }
        Array<Bitset> bitsets(8192);

        Array<Byte> data(256*10*37);

        String s = File("Image (3).raw", true).contents();
        for (int row = 0; row < 37; ++row) {
            for (int y = 0; y < 10; ++y) {
                for (int x = 0; x < 256; ++x) {
                    data[row + (x+y*256)*37] = (s[row*256*10 + y*256 + (x^1)] == 0 ? 0x00 : 0xff);
                }
            }
        }
#if 0
        for (int x = 0; x < 2560; ++x) {
            bool empty = true;
            for (int row = 0; row < 37; ++row) {
                if (data[row + x*37] != 0)
                    empty = false;
            }
            if (empty) {
                for (int row = 0; row < 37; ++row) {
                    data[row + x*37] = 0x80;
                }
                int yy = x/256;
                int xx = (x & 0xff) ^1;
                for (int i = 0; i < 8; ++i)
                    printf("%c", ((xx >> (7 - i)) & 1) + '0');
                printf(" ");
                printf("%c\n", yy + 'A');
            }
        }
#endif

        //for (int row = 0; row < 37; ++row) {
        //    String s = File("blobs\\blob_row_" + decimal(row, 2) + ".txt", true).contents();
        //    int p = 0;
        //    for (int y = 0; y < 10; ++y) {
        //        for (int x = 0; x < 256; ++x) {
        //            //data[x+y*256+row*2560] = s[p];
        //            //data[row + (x*10+y)*37] = s[p];
        //            data[row + (x+y*256)*37] = (s[p] == '0' ? 0x00 : 0xff);
        //            ++p;
        //        }
        //        p += 2;
        //    }
        //}
        File("blob.raw").openWrite().write(data);

        //Array<Byte> ent(37*37);
        //for (int start = 0; start < 37; ++start) {
        //    for (int end = 0; end < start; ++end) {
        //        printf("     ");
        //        ent[end + start*37] = 0;
        //    }
        //    for (int end = start; end < 37; ++end) {
        //        uint64_t value[2560];
        //        uint64_t count[2560];
        //        int used = 0;
        //        for (int p = 0; p < 2560; ++p) {
        //            uint64_t v = 0;
        //            for (int bit = start; bit <= end; ++bit)
        //                if (data[p*37 + bit] != 0)
        //                    v += (1 << (bit - start));
        //            int found = -1;
        //            for (int search = 0; search < used; ++search)
        //                if (value[search] == v) {
        //                    found = search;
        //                    break;
        //                }
        //            if (found == -1) {
        //                found = used;
        //                value[found] = v;
        //                count[found] = 0;
        //                ++used;
        //            }
        //            ++count[found];
        //        }
        //        double entropy = 0;
        //        for (int i = 0; i < used; ++i) {
        //            double probability = count[i]/2560.0;
        //            entropy -= probability*log(probability)/log(2);
        //        }
        //        double entropy_per_bit = entropy/(end + 1 - start);
        //        printf("%0.2f ",entropy_per_bit);
        //        ent[end + start*37] = static_cast<int>(entropy_per_bit*0xff);
        //    }
        //    printf("\n");
        //}
        //File("entropy.raw").openWrite().write(ent);
#if 0
        for (int x = 0; x < 2560 - 2; ++x) {
            for (int r1p = 0; r1p < 37 - 7; ++r1p) {
                for (int r2p = 0; r2p < 37 - 7; ++r2p) {
                    int dst[3];
                    int src[3];
                    for (int y = 0; y < 3; ++y) {
                        dst[y] = 0;
                        for (int b1 = 0; b1 < 7; ++b1)
                            if (data[(x + y)*37 + b1 + r1p] != 0)
                                dst[y] += (1 << b1);
                        src[y] = 0;
                        for (int b2 = 0; b2 < 7; ++b2)
                            if (data[(x + y)*37 + b2 + r2p] != 0)
                                src[y] += (1 << b2);
                    }
                    //if (dst[0] == src[1] && dst[1] == src[2] && dst[2] == src[0] && dst[0] != dst[1] && dst[1] != dst[2] && dst[2] != dst[0]) {
                    if (dst[0] == src[2] && dst[1] == src[0] && dst[2] == src[1] && dst[0] != dst[1] && dst[1] != dst[2] && dst[2] != dst[0]) {
                            printf("Potential XCHG at %03x offsets %i,%i regnums %02x,%02x,%02x\n",2559 - x - 2,r1p,r2p, dst[0], dst[1], dst[2]);
                    }
                }
            }
        }
#endif
        int usedS[64];
        int usedD[128];
        int usedA[64];
        int usedAJ[128];
        int usedBus[64];
        int usedOp[8];
        int usedSub[4];
        int lastS[64];
        int lastD[128];
        int lastA[64];
        int lastAJ[128];
        int lastBus[64];
        for (int i = 0; i < 64; ++i) {
            usedS[i] = 0;
            usedD[i] = 0;
            usedD[i + 64] = 0;
            usedA[i] = 0;
            usedAJ[i] = 0;
            usedAJ[i + 64] = 0;
            usedBus[i] = 0;
        }
        for (int i = 0; i < 4; ++i) {
            usedOp[i] = 0;
            usedOp[i + 4] = 0;
            usedSub[i] = 0;
        }

        String regnames_source[64];
        for (int i = 0; i < 64; ++i)
            regnames_source[i] = "      "; //"[-" + hex(i,2,false) + "-]";
        String regnames_dest[128];
        for (int i = 0; i < 128; ++i)
            regnames_dest[i] = "      "; //"(-" + hex(i, 2, false) + "-)";
        String regnames_alu[128];
        for (int i = 0; i < 64; ++i)
            regnames_alu[i] = "      "; //"<-" + hex(i, 2, false) + "->";
        String alujump_names[128];
        for (int i = 0; i < 128; ++i)
            alujump_names[i] = "      "; //"{-" + hex(i, 2, false) + "-}";
        String busop_names[64];
        for (int i = 0; i < 64; ++i)
            busop_names[i] = "    "; //<" + hex(i, 2, false) + ">";
        String opnames[8];
        for (int i = 0; i < 8; ++i)
            opnames[i] = "   "; //[" + hex(i, 1, false) + "]";
        String subopnames[4];
        for (int i = 0; i < 4; ++i)
            subopnames[i] = "   "; //(" + hex(i, 1, false) + ")";
        String testnames[64];
        for (int i = 0; i < 64; ++i)
            testnames[i] = "<" + hex(i, 2, false) + ">";

        regnames_source[0x00] = "EAX   "; // 3  AAA/AAS/DAA/DAS and REP STOS    Could be EAX - STOS copies to OPR_W but that needs to know size anyway
        regnames_dest  [0x00] = "EAX   "; // 3
        regnames_dest  [0x40] = "AX    "; // 2  AAA/AAS and FSTSW AX
        regnames_alu   [0x00] = "EAX   "; // 1  902
        regnames_source[0x01] = "ECX   "; // 4  rot/SHxD
     // regnames_dest  [0x01] = "ECX   ";
     // regnames_dest  [0x41] = "CX    ";
        regnames_alu   [0x01] = "ECX   "; // 2  rot
        regnames_source[0x02] = "EDX   "; // 8
        regnames_dest  [0x02] = "EDX   "; // 1  BOOTUP
     // regnames_dest  [0x42] = "DX    ";
        regnames_alu   [0x02] = "EDX   ";
     // regnames_source[0x03] = "EBX   ";
     // regnames_dest  [0x03] = "EBX   ";
     // regnames_dest  [0x43] = "BX    ";
        regnames_alu   [0x03] = "EBX   ";
        regnames_source[0x04] = "ESP   "; // 25
        regnames_dest  [0x04] = "ESP   "; // 2
     // regnames_dest  [0x44] = "SP    ";
        regnames_alu   [0x04] = "ESP   "; // 1  60C
        regnames_source[0x05] = "EBP   "; // 4  ENTER/LEAVE
        regnames_dest  [0x05] = "EBP   "; // 2  ENTER/LEAVE
        regnames_dest  [0x45] = "BP    "; // 2  ENTER/LEAVE
        regnames_alu   [0x05] = "EBP   ";
        regnames_source[0x06] = "ESI   "; // 10 [REP] MOVS/[REP] CMPS/[REP] LODS/[REP] OUTS
     // regnames_dest  [0x06] = "ESI   ";
     // regnames_dest  [0x46] = "SI    ";
        regnames_alu   [0x06] = "ESI   "; // 2
        regnames_source[0x07] = "EDI   "; // 19 PUSHAd(?)/[REP] MOVS/[REP] CMPS/[REP] SCAS/[REP] STOS/[REP] INS
     // regnames_dest  [0x07] = "EDI   ";
     // regnames_dest  [0x47] = "DI    ";
        regnames_alu   [0x07] = "EDI   "; // 2
        regnames_source[0x08] = "EIP   "; // 44
        regnames_dest  [0x08] = "EIP   "; // 3  RPTI, DR/TR, ??? - always used with [-16-]
        regnames_dest  [0x48] = "IP    "; // 1  649
        regnames_alu   [0x08] = "IMM8  "; // 10 JCXZ/LOOP/LOOPE/LOOPNE/Jcond/JMP/CALL/ENTER/VERR/VERW
        regnames_source[0x09] = "EFLAGS"; // 22 LAHF , RPTI,
        regnames_dest  [0x09] = "EFLAGS"; // 10
        regnames_dest  [0x49] = "FLAGS "; // 3  IRETd/POPFd/INT ib
        regnames_alu   [0x09] = "IMM   "; // 12
        regnames_source[0x0A] = "CR0   "; // 30        MSW is the low 16 bits of CR0
        regnames_dest  [0x0A] = "CR0   "; // 7
     // regnames_dest  [0x4A] = "      ";
     // regnames_alu   [0x0A] = "      ";
        regnames_source[0x0B] = "TMPB  "; // 67
        regnames_dest  [0x0B] = "TMPB  "; // 125
     // regnames_dest  [0x4B] = "      ";
        regnames_alu   [0x0B] = "TMPB  "; // 96
        regnames_source[0x0C] = "TMPC  "; // 31
        regnames_dest  [0x0C] = "TMPC  "; // 73
     // regnames_dest  [0x4C] = "      ";
        regnames_alu   [0x0C] = "TMPC  "; // 52
        regnames_source[0x0D] = "TMPD  "; // 10
        regnames_dest  [0x0D] = "TMPD  "; // 44
        regnames_dest  [0x4D] = "DESPTR"; // 7  LIDTGDT_COMMON/LD_SEGREG_PASS/PRESENT_TSS/PROT_TESTS_PASSED/PROT_TESTS_PASSED2/PASSED_0B_0D   descriptor mentioned in dest of last SPTR
        regnames_alu   [0x0D] = "TMPD  "; // 37
        regnames_source[0x0E] = "TMPE  "; // 29
        regnames_dest  [0x0E] = "TMPE  "; // 22
        regnames_dest  [0x4E] = "DESSDT"; // 6  LTR/?              Either DESGDT or DESIDT depending on the Table Indictor bit in the selector? Which selector? SLCTR? source?
     // regnames_alu   [0x0E] = "      ";
        regnames_source[0x0F] = "TMPF  "; // 9    Low 16 bits of TMPF are used for FP (opcode?). Bits 16-18 are used for error code
        regnames_dest  [0x0F] = "TMPF  "; // 11
     // regnames_dest  [0x4F] = "      ";
     // regnames_alu   [0x0F] = "      ";
        regnames_source[0x10] = "FLAGSB"; // 8
        regnames_dest  [0x10] = "FLAGSB"; // 14
        regnames_dest  [0x50] = "AL    "; // 5  SALC/DAA/DAS/AAM/AAD
     // regnames_alu   [0x10] = "      ";
        regnames_source[0x11] = "TMPH  "; // 25
        regnames_dest  [0x11] = "TMPH  "; // 18
     // regnames_dest  [0x51] = "CL    ";
     // regnames_alu   [0x11] = "      ";
        regnames_source[0x12] = "TMPG  "; // 43
        regnames_dest  [0x12] = "TMPG  "; // 32
     // regnames_dest  [0x52] = "DL    ";
     // regnames_alu   [0x12] = "      ";
        regnames_source[0x13] = "SLCTR2"; // 32          Related to SLCTR?
        regnames_dest  [0x13] = "SLCTR2"; // 11
     // regnames_dest  [0x53] = "BL    ";
     // regnames_alu   [0x13] = "      ";
        regnames_source[0x14] = "COUNTR"; // 63
        regnames_dest  [0x14] = "MDTMP4"; // 11          Could this be the same as (-1D-)/MDTMP? Some routines outside of mul/div seem to use [-1D-]/(-14-) as a temp. Maybe (-1D-) has special powers?
        regnames_dest  [0x54] = "AH    "; // 3  LAHF/AAM/AAD
        regnames_alu   [0x14] = "0x3ddc0c2c"; // 1  9B8  Expected result of built-in self-test (BIST)
        regnames_source[0x15] = "PROTUN"; // 18  protection test unit
        regnames_dest  [0x15] = "PROTUN"; // 59
     // regnames_dest  [0x55] = "CH    ";
        regnames_alu   [0x15] = "0x0303"; // 1  9B5
        regnames_source[0x16] = "TMPeIP"; // 23
        regnames_dest  [0x16] = "TMPeIP"; // 9
     // regnames_dest  [0x56] = "DH    ";
        regnames_alu   [0x16] = "0x37fd7"; // 5
        regnames_source[0x17] = "TMPeSP"; // 33
        regnames_dest  [0x17] = "TMPeSP"; // 7
     // regnames_dest  [0x57] = "BH    ";
        regnames_alu   [0x17] = "0x4000"; // 7
        regnames_source[0x18] = "DR6   "; // 9
        regnames_dest  [0x18] = "DR6   "; // 9
        regnames_dest  [0x58] = "DESCSW"; // 5     Descriptor that uses base and limit from CS descriptor but is writeable
        regnames_alu   [0x18] = "~0x200"; // 1  790
        regnames_source[0x19] = "DR7   "; // 8
        regnames_dest  [0x19] = "DR7   "; // 7
        regnames_dest  [0x59] = "DESCOD"; // 23    Descriptor for eIP changes (DES_CS is only used for updating the CS descriptor)
        regnames_alu   [0x19] = "8     "; // 3
        regnames_source[0x1A] = "CSOPCD"; // 7     CSOPCD, TMPN and TMPO are only used by FSAVE.
        regnames_dest  [0x1A] = "CSOPCD"; // 6
        regnames_dest  [0x5A] = "DESSTK"; // 24    Descriptor for stack reads/writes (DES_SS is only used for updating the stack descriptor)
        regnames_alu   [0x1A] = "0x40  "; // 2
        regnames_source[0x1B] = "FSVeIP"; // 2
        regnames_dest  [0x1B] = "FSVeIP"; // 7
     // regnames_dest  [0x5B] = "      ";
        regnames_alu   [0x1B] = "0xf0000"; // 2
        regnames_source[0x1C] = "OPROFF"; // 12
        regnames_dest  [0x1C] = "OPROFF"; // 6
        regnames_dest  [0x5C] = "eSP   "; // 39
        regnames_alu   [0x1C] = "0x0d  "; // 9
        regnames_source[0x1D] = "MDTMP "; // 15    quotient after division, product after multiplication?
        regnames_dest  [0x1D] = "MDTMP "; // 7     one of the multiplier inputs?
        regnames_dest  [0x5D] = "DES_OS"; // 37    strings, XLAT,  - but used in "POP m" and SGDT with no ALU op   - like IND_DS but "overrideable segment" (uses DS for bus operation unless a segment override is active, in which case use that segment)
        regnames_alu   [0x1D] = "0x5d  "; // 4
        regnames_source[0x1E] = "SIGMA "; // 533
     // regnames_dest  [0x1E] = "      ";
        regnames_dest  [0x5E] = "DES_SR"; // 11    Like other IND_ registers but are written to when segment registers are updated
        regnames_alu   [0x1E] = "0x800000f8"; // 3
        regnames_source[0x1F] = "IMM   "; // 36
     // regnames_dest  [0x1F] = "      ";
        regnames_dest  [0x5F] = "eIP   "; // 9
        regnames_alu   [0x1F] = "0x800000fc"; // 5

        regnames_source[0x20] = "ES    "; // 5
        regnames_dest  [0x20] = "ES    "; // 7
        regnames_dest  [0x60] = "DES_ES"; // 33
        regnames_alu   [0x20] = "0x70  "; // 2
        regnames_source[0x21] = "CS    "; // 49
        regnames_dest  [0x21] = "CS    "; // 6
        regnames_dest  [0x61] = "DES_CS"; // 25
        regnames_alu   [0x21] = "0x73  "; // 1  9A9
        regnames_source[0x22] = "SS    "; // 26
        regnames_dest  [0x22] = "SS    "; // 5
        regnames_dest  [0x62] = "DES_SS"; // 8
        regnames_alu   [0x22] = "0x1ff "; // 1  336
        regnames_source[0x23] = "DS    "; // 6
        regnames_dest  [0x23] = "DS    "; // 6
        regnames_dest  [0x63] = "DES_DS"; // 5
        regnames_alu   [0x23] = "0x8200"; // 11
        regnames_source[0x24] = "FS    "; // 4
        regnames_dest  [0x24] = "FS    "; // 4
        regnames_dest  [0x64] = "DES_FS"; // 4
        regnames_alu   [0x24] = "0x47  "; // 1  091
        regnames_source[0x25] = "GS    "; // 8
        regnames_dest  [0x25] = "GS    "; // 4
        regnames_dest  [0x65] = "DES_GS"; // 4
        regnames_alu   [0x25] = "3     "; // 16
        regnames_source[0x26] = "LDTR  "; // 6
        regnames_dest  [0x26] = "LDTR  "; // 4
        regnames_dest  [0x66] = "DESLDT"; // 5
        regnames_alu   [0x26] = "6     "; // 5
     // regnames_source[0x27] = "      ";
     // regnames_dest  [0x27] = "      ";
        regnames_dest  [0x67] = "DESGDT"; // 3
        regnames_alu   [0x27] = "0xffff0000"; // 9
        regnames_source[0x28] = "eAX_AL"; // 15  AAM/AAD/MUL/IMUL/DIV/IDIV/CWD/CDQ/[REP] SCAS/XLATB/STOS/OUT
        regnames_dest  [0x28] = "eAX_AL"; // 7   MUL/IMUL/DIV/IDIV/[REP] LODS/XLATB/IN
        regnames_dest  [0x68] = "DESIDT"; // 5
        regnames_alu   [0x28] = "0x60  "; // 2
        regnames_source[0x29] = "TR    "; // 9
        regnames_dest  [0x29] = "TR    "; // 5
        regnames_dest  [0x69] = "DES_TR"; // 28

        regnames_alu   [0x29] = "0x7ff "; // 3
        regnames_source[0x2A] = "eDX_AH"; // 6   SAHF/AAD/DIV/IDIV
        regnames_dest  [0x2A] = "eDX_AH"; // 4   MUL/IMUL/DIV/IDIV/CWD/CDQ
        regnames_dest  [0x6A] = "DESABS"; // 14  bus operation in which IND is placed on the address bus without any paging translation - used for halt/shutdown/interrupt acknowledge bus cycles and other debugging information placed on the address bus
        regnames_alu   [0x2A] = "9     "; // 4
        regnames_source[0x2B] = "CR2   "; // 1  378  Contains a value called Page Fault Linear Address (PFLA). When a page fault occurs, the address the program attempted to access is stored in the CR2 register.
        regnames_dest  [0x2B] = "CR2   "; // 4
        regnames_dest  [0x6B] = "DES_IO"; // 33  used for access to IO ports        DES_IO
        regnames_alu   [0x2B] = "0x29  "; // 9   LDCNTR in reset (002)
     // regnames_source[0x2C] = "      ";
     // regnames_dest  [0x2C] = "      ";
        regnames_dest  [0x6C] = "DESERR"; // 1   ACCESS_VIOLATI  - descriptor that caused the fault
        regnames_alu   [0x2C] = "7     "; // 13
        regnames_source[0x2D] = "OPR_R "; // 156   It's tempting to call both OPR_R and OPR_W just OPR, but they do seem to be separate: look at 07E - OPR_R is available here (used on uop after DLY in other instructions) and used at line 080 but OPR_W is overwritten with eIP...
        regnames_dest  [0x2D] = "OPR_W "; // 102   ...however, on line 083 OPR_R is never copied to OPR_W. Maybe "wr" writes from OPR_R?
     // regnames_dest  [0x6D] = "      ";
        regnames_alu   [0x2D] = "0x0f  "; // 5
        regnames_source[0x2E] = "IRF2  "; // 53
     // regnames_dest  [0x2E] = "      ";
     // regnames_dest  [0x6E] = "      ";
        regnames_alu   [0x2E] = "0x65  "; // 3
        regnames_source[0x2F] = "BIST1 "; // 1  9B5
     // regnames_dest  [0x2F] = "      ";
     // regnames_dest  [0x6F] = "      ";
        regnames_alu   [0x2F] = "0x1f  "; // 2
        regnames_source[0x30] = "BIST2 "; // 1  9B7
     // regnames_dest  [0x30] = "      ";
     // regnames_dest  [0x70] = "DR0   ";
        regnames_alu   [0x30] = "1     "; // 43
        regnames_source[0x31] = "eCX   "; // 11
        regnames_dest  [0x31] = "eCX   "; // 16
     // regnames_dest  [0x71] = "DR1   ";
        regnames_alu   [0x31] = "2     "; // 13
     // regnames_source[0x32] = "      ";
        regnames_dest  [0x32] = "COUNTR"; // 60  IRF index? Temp related to [-14-]?
     // regnames_dest  [0x72] = "DR2   ";
        regnames_alu   [0x32] = "0x10  "; // 11
     // regnames_source[0x33] = "      ";
     // regnames_dest  [0x33] = "      ";
     // regnames_dest  [0x73] = "DR3   ";
        regnames_alu   [0x33] = "4     "; // 16
     // regnames_source[0x34] = "      ";
     // regnames_dest  [0x34] = "      ";
     // regnames_dest  [0x74] = "TR4   ";
        regnames_alu   [0x34] = "-1    "; // 10
        regnames_source[0x35] = "SLCTR "; // 6    selector to load (pointer into LDT/GDT)?
        regnames_dest  [0x35] = "SLCTR "; // 33   selector to load (pointer into LDT/GDT)?
     // regnames_dest  [0x75] = "TR5   ";
        regnames_alu   [0x35] = "-2    "; // 3
        regnames_source[0x36] = "EA    "; // 3    not eSI
        regnames_dest  [0x36] = "eSI   "; // 11
     // regnames_dest  [0x76] = "TR6   ";
        regnames_alu   [0x36] = "0xffff"; // 39
     // regnames_source[0x37] = "eDI   ";
        regnames_dest  [0x37] = "eDI   "; // 13
     // regnames_dest  [0x77] = "TR7   ";
        regnames_alu   [0x37] = "-4    "; // 8
        regnames_source[0x38] = "0     "; // 106
        regnames_dest  [0x38] = "FLAGSL"; // 1  0DA      SAHF
        regnames_dest  [0x78] = "LATTTF"; // 1  PAGE_FAULT         Related to paging      linear address that triggered the fault
     // regnames_alu   [0x38] = "      ";
        regnames_source[0x39] = "IRF   "; // 7
        regnames_dest  [0x39] = "IRF   "; // 38    Indirect access to Register File
        regnames_dest  [0x79] = "PGUNUS"; // 1  unused?            Related to paging      linear address that triggered the fault again (value ends up in CR2 anyway like LATTTF - could have had a different meaning here)
        regnames_alu   [0x39] = "WORDSZ"; // 25
     // regnames_source[0x3A] = "      ";
        regnames_dest  [0x3A] = "COUNT5"; // 6     COUNTR except that all but low 5 bits are masked off      RCL/RCR/ENTER_COMMON/CALL_FAR_PM_GATE
        regnames_dest  [0x7A] = "PFERRC"; // 2  PAGE_FAULT/unused? Related to paging      page fault error code
        regnames_alu   [0x3A] = "NEGWSZ"; // 22    negative word size in bytes (-2 or -4) (0xfffffffe or 0xfffffffc)
        regnames_source[0x3B] = "OPCODE"; // 8     As stored by FSAVE/FSTENV - 0 to 0x7ff
     // regnames_dest  [0x3B] = "      ";
        regnames_dest  [0x7B] = "PDBR  "; // 11                    Page directory base register (aka CR3) - write to this to flush TLB cache
        regnames_alu   [0x3B] = "INCREM"; // 32    eSI/eDI increment for string instructions (+/- 1/2/4)
        regnames_source[0x3C] = "SEGREG"; // 3
        regnames_dest  [0x3C] = "SEGREG"; // 9
     // regnames_dest  [0x7C] = "      ";
        regnames_alu   [0x3C] = "BITS_V"; // 21    one less than word size in bits (15 or 31)
        regnames_source[0x3D] = "DSTREG"; // 37
        regnames_dest  [0x3D] = "DSTREG"; // 30
        regnames_dest  [0x7D] = "PAGER5"; // 12
        regnames_alu   [0x3D] = "DSTREG"; // 12
        regnames_source[0x3E] = "SRCREG"; // 21
        regnames_dest  [0x3E] = "SRCREG"; // 22
     // regnames_dest  [0x7E] = "      ";
        regnames_alu   [0x3E] = "SRCREG"; // 7
        regnames_source[0x3F] = "-1    "; // 95    SALC
     // regnames_dest  [0x3F] = "      ";
        regnames_dest  [0x7F] = "      "; // 550
        regnames_alu   [0x3F] = "0     ";
        alujump_names[0x00] = "+-&|^ "; // 5
        alujump_names[0x01] = "++--~-"; // 2
        alujump_names[0x02] = "<<>>? "; // 13
        alujump_names[0x03] = "CMPTST"; // 5
        alujump_names[0x04] = "IMCS  "; // 1  1BA  IMUL Correct Sign. Corrects for sign regardless of opcode
        alujump_names[0x05] = "SERECO"; // 4       BTS/BTR/BTC   SEt, REset, COmplement according to which opcode used. Actual operation is either ALU1 OR ALU2, ALU1 AND NOT ALU2 or ALU1 XOR ALU2. There's a shift in there too?
        alujump_names[0x06] = "SZ_EXT"; // 4       MOVZX/MOVSX/CBW/CWDE
        alujump_names[0x07] = "SZ_EX2"; // 1  1AC  MUL/IMUL rm - binary operation. Correct upper part of multiplication for sign in IMUL case. Not sure how this is different from SZ_EXT
        alujump_names[0x08] = "AND   "; // 84      ok
        alujump_names[0x09] = "OR    "; // 41      probably not SUB - used to clear (or set?) resume flag in RPTI? - AND NOT? Also used in JMP cp
        alujump_names[0x0A] = "XOR   "; // 13
        alujump_names[0x0B] = "SIGN  "; // 4       probably not CMP - BT/BTS/BTR/BTC/CWD/CDQ/77a  SIGMA is sign-extension of input
        alujump_names[0x0C] = "ADD   "; // 123     ok
        alujump_names[0x0D] = "ADC   "; // 2       Used in AAM and AAD
        alujump_names[0x0E] = "SUB   "; // 36      probably not ADC - SUB?
        alujump_names[0x0F] = "CMP   "; // 8       porbably not SBB - SUB? CMP?
        alujump_names[0x10] = "SHIFT "; // 72
        alujump_names[0x11] = "BITTST"; // 11      BT/BTS/BTR/BTC/BSF -  does a shift and copies the lowest bit into the carry flag?
        alujump_names[0x12] = ">><<? "; // 12
        alujump_names[0x13] = "FLGOPS"; // 1  7EF  flag operation determined by opcode: CLC/STC/CLD/STD/CMC
        alujump_names[0x14] = "PASS  "; // 55
        alujump_names[0x15] = "PASS2 "; // 105
        alujump_names[0x16] = "AAAAAS"; // 1  189
        alujump_names[0x17] = "DAADAS"; // 1  18D
        alujump_names[0x18] = "PREDIV"; // 1  1D8  store signs of inputs (
        alujump_names[0x19] = "IDIV1 "; // 1  1DB
        alujump_names[0x1A] = "IDIV2 "; // 1  1DE
        alujump_names[0x1B] = "IMUL3 "; // 2
        alujump_names[0x1C] = "IMUL4 "; // 1  1B5
        alujump_names[0x1D] = " DIV5 "; // 3
     // alujump_names[0x1E] = "      ";
        alujump_names[0x1F] = " DIV7 "; // 3
     // alujump_names[0x20] = "      ";
     // alujump_names[0x21] = "      ";
     // alujump_names[0x22] = "      ";
        alujump_names[0x23] = "CLRNMI"; // 3       IRETd/LOADALL386      Seems likely that {-20-}..{-3D-} set flags. Low bit is set/reset?      Clear NMI flag? CLRNMI?   Set I+-V direction to +?
        alujump_names[0x24] = "BITS8 "; // 7       SETcond/SAHF/AAA/AAS/DAA/DAS/AAM/AAD    - SETcond doesn't change any flags so it's not a flag set/reset. Unconditional at start of each of these instructions      BITS8 ?
        alujump_names[0x25] = "BITS16"; // 17      Set CPU to 16-bit mode for remainder of instruction
        alujump_names[0x26] = "SETNMI"; // 1  836
        alujump_names[0x27] = "BITS32"; // 51      Set CPU to 32-bit mode for remainder of instruction
        alujump_names[0x28] = "CLZF  "; // 5       BSF/BSR/ARPL/???     clear zero flag?
        alujump_names[0x29] = "SEZF  "; // 4       ARPL/LSL/            set zero flag?
        alujump_names[0x2A] = "BITSDE"; // 8       PORTIO_PROTCHK/FSTENV_FSAVE/PRESENT_TSS/PROT_TESTS_PASSED/LSL_GRANULARITY_*/LAR_LSL_VERRW/FAULT           restore instruction's original bit size (undo BITS8/BITS16/BITS32)?
        alujump_names[0x2B] = "ICEBRK"; // 5       GD_HIT/LOADALL386/ICEBP/BREAKPOINT/ICE_PIN           signal to ICE to say that we're either hitting a breakpoint (ICEBP, BREAKPOINT, SINGLE_STEP, GD_HIT, ICE_PIN) or LOADALL is completing
        alujump_names[0x2C] = "SCNTFF"; // 9       DIVIDE_ERROR/TRIPLE_FAULT/DOUBLE_FAULT/NO_PRIVILEGE/unused?/GENERAL_FAULT5/FAULT_ERR_CODE/PAGE_FAULT/BOOTUP  Set flag to say that we're processing a contributory fault (for double/triple fault detection)  CONTRIBUTORY_FAULT = true;
        alujump_names[0x2D] = "CINTLA"; // 1       HARDWARE_IRQ - clear INT latch? (If INT line goes inactive and active again after executing this, it's a new interrupt)
        alujump_names[0x2E] = "CLI   "; // 3       TRAPGATE286, TRAPGATE386, real mode part of INTERRUPT  - clear interrupt flag. No CLI if interrupt through task gate?
        alujump_names[0x2F] = "CLT   "; // 4       MORE_PRIV_INT, SAVE_TASK, TRAP_INT_GATE, STOREALL      - clear trap flag. Not sure why this is done on SAVE_TASK
        alujump_names[0x30] = "STSSAF"; // 5       TSS_ACCESS_FLAG = true;     set TSS access flag
     // alujump_names[0x31] = "      ";
        alujump_names[0x32] = "SINTHW"; // 2       CALL_FAR_PM_GATE/INTERRUPT_X (HARDWARE_IRQ/NMI/STRING_CORRECT/SINGLE_STEP)       INTERRUPT_HW = true
        alujump_names[0x33] = "SMISC1"; // 8       MISC1 = true   - sets flag to say we're doing an FLDENV and not FRSTOR (as well as other purposes)
        alujump_names[0x34] = "STSKS "; // 2       TASK_SAVED = true
        alujump_names[0x35] = "SMISC2"; // 4       MISC2 = true
        alujump_names[0x36] = "SERRCF"; // 5       ERROR_CODE_FLAG = true;  Set flag to say that we're processing certain kinds of fault (for double/triple fault detection?)  DOUBLE_FAULT, NO_PRIVILEGE, unused?, FAULT_ERR_CODE, PAGE_FAULT    Set flag to say that we're going to push an error code onto the stack
        alujump_names[0x37] = "SNOFLT"; // 1       NO_FAULT = true          LAR_LSL_VERRW
        alujump_names[0x38] = "FLGSBA"; // 13      FLAGS_BACKED_UP = true   FLAGS9 backup is active, restore from FLAGSB if a fault is triggered before the end of the instruction
        alujump_names[0x39] = "SREPF "; // 1  1F9  REPF = true      REP MOVS
        alujump_names[0x3A] = "CTSSAF"; // 2       TSS_ACCESS_FLAG = false;     MORE_PRIVILEGE, FAULT
        alujump_names[0x3B] = "CTSKS "; // 3       TASK_SAVED = false     TASK_RETURN, LOAD_TASK_COMM (just before SET_TSKSWTCHED in either nested or non-nested cases)
        alujump_names[0x3C] = "CMISC2"; // 3       CMISC2 i.e. MISC2 = false
        alujump_names[0x3D] = "CREPF "; // 1       REPF = false           STRING_CORRECT
        alujump_names[0x3E] = "LDCNTR"; // 45      load COUNT register with value in alu2 source field?
        alujump_names[0x3F] = "DECNTR"; // 47      decrement COUNT register?  ALU2 always empty
        alujump_names[0x40] = "J16BIT"; // 8       PORTIO_PROTCHK      - jump if we're doing 286 compatible stuff - see SAVE_TASK
        alujump_names[0x41] = "JNcond"; // 3
        alujump_names[0x42] = "JFPUOK"; // 7       Jump if BUSY# pin is inactive (high) and ERROR# pin is inactive (high)
        alujump_names[0x43] = "LOOPnE"; // 5       LOOPE/LOOPNE logic - jump if COUNTR is non-zero and if ZF is 0 (LOOPNE) or 1 (LOOPE)
        alujump_names[0x44] = "JCNTZ "; // 16      jump if COUNTR zero? (Jump if interrupt requested? - no)
        alujump_names[0x45] = "JCNTNZ"; // 22      JCXZ and others - jump if COUNTR non-zero?
        alujump_names[0x46] = "JCT4N1"; // 9       jump if low 4 bits of COUNTR are not 1? JCT4N1?
        alujump_names[0x47] = "JCNZNI"; // 1  203  jump if COUNTR != 0 and no interrupt requested (only used in REP MOVS)
        alujump_names[0x48] = "JCNT1 "; // 6       LOOP - jump if COUNTR == 1
        alujump_names[0x49] = "JCNTN1"; // 3       jump if COUNTR != 1 ?
     // alujump_names[0x4A] = "      ";
        alujump_names[0x4B] = "JIO_OK"; // 13      jump if CPL <= IOPL - skips permission check for IN/OUT and avoids falling through to #GP(0) in POPFd/PUSHFd/CLI/STI/INT/IRETd
        alujump_names[0x4C] = "JEXTFT"; // 1  878  Jump if external (to program) fault (set bit 0 (EXT) in fault error code)
        alujump_names[0x4D] = "JSTSKL"; // 2       LOAD_TASK,   - condition by which we skip the write of the backlink - how do we decide whether to set NT? Does it happen when we have a TSS?   - jump if skip task link  if (!MISC1 && !INTERRUPT_HW) goto
        alujump_names[0x4E] = "JNPERQ"; // 16      Jump if PEREQ pin is inactive (low)?
        alujump_names[0x4F] = "JFERR#"; // 6       Jump if ERROR# pin is active (low)?
        alujump_names[0x50] = "JTSSAF"; // 2       Jump if TSS access flag is set                                  if (!TSS_ACCESS_FLAG)
        alujump_names[0x51] = "JG    "; // 10      Jump if greater - can figure out sign from looking at 882
        alujump_names[0x52] = "JINTSW"; // 1       Jump if INTERRUPT_HW is not set                                 if (!INTERRUPT_HW)
        alujump_names[0x53] = "JMISC1"; // 8       Jump if flag set by SMISC1 is set
        alujump_names[0x54] = "JNTSKS"; // 1  74C    Jump if "task has been saved" flag is clear?
        alujump_names[0x55] = "JMISC2"; // 5       Jump if flag set by SMISC2 is set (or if it's not set)?
        alujump_names[0x56] = "JNERRC"; // 3       Jump if no error code (i.e. jump if SERRCF was not executed)    if (!ERROR_CODE_FLAG)
        alujump_names[0x57] = "JNOFLT"; // 1       Jump if we're executing LAR_LSL_VERRW and should not fault      if (NO_FAULT)    FAULT_ERR_CODE - ends instruction with no fault if set
        alujump_names[0x58] = "JNFLGB"; // 1  890   Jump if flags were not backed up (no FLGSBA operation was performed)?
        alujump_names[0x59] = "JREP  "; // 1  89C   Jump if REP prefix active?
        alujump_names[0x5A] = "JMP   "; // 88
        alujump_names[0x5B] = "JNT   "; // 1  673   Jump if NT (Nested Task) bit is 1
        alujump_names[0x5C] = "JNO   "; // 1       INTO
        alujump_names[0x5D] = "JNC   "; // 6   only used by SALC
        alujump_names[0x5E] = "JICEWT"; // 7        FPU_FLAG/TRIPLE_FAULT/LAR_LSL_VERRW/FAULT/PAGE_FAULT/ICE_SINGLESTEP/ICE_PIN  - used in busy wait loops for some external signal, presumably so that ICE can pause the CPU at these points, essentially
        alujump_names[0x5F] = "JNOINT"; // 14  jump if interrupt not pending?
     // alujump_names[0x60] = "      ";
        alujump_names[0x61] = "PTSAV1"; // 24  source always 0, busop always SPTR
     // alujump_names[0x62] = "      ";
        alujump_names[0x63] = "PTSAV3"; // 3   source always 0, busop always SPTR
     // alujump_names[0x64] = "      ";
     // alujump_names[0x65] = "      ";
     // alujump_names[0x66] = "      ";
        alujump_names[0x67] = "PTSAV7"; // 11  source always 0, busop always SPTR   - used for SS tests, jump/call gate dests?
        alujump_names[0x68] = "PTOVRR"; // 4   test is overrideable using PTSAV*
        alujump_names[0x69] = "PTGATE"; // 2   (re-tests last descriptor with another test)
        alujump_names[0x6A] = "PTSELA"; // 22  used for selectors
        alujump_names[0x6B] = "PTGEN "; // 39  general, used for FPU tests and many SETs
     // alujump_names[0x6C] = "      ";
     // alujump_names[0x6D] = "      ";
        alujump_names[0x6E] = "PTSELE"; // 15  also often used for selectors
        alujump_names[0x6F] = "PTF   "; // 11  various random things
        alujump_names[0x70] = "LCALL "; // 69
        alujump_names[0x71] = "LJUMP "; // 64
        alujump_names[0x72] = "LJMPNP"; // 24     long jump if privilege level not equal to 0    (PM && CPL > 0) in CLTS/HLT/LIDT/LGDT/LLDT/LMSW/CRn/DRn/TRn/LTR/LOADALL386, (PM && CPL > IOPL) in PORTIO_PROTCHK
        alujump_names[0x73] = "LJMPVM"; // 6      IRETd (p)/???      long jump only in virtual 8086 mode
        alujump_names[0x74] = "LJMPP "; // 17     long jump only in protected mode
        alujump_names[0x75] = "ICEENT"; // 2      ICE_SINGLESTEP/ICE_PIN -  send some signal to ICE?  Always happens right before STOREALL - enter ICE mode
        alujump_names[0x76] = "ICEEXT"; // 1      LOADALL386             - tell ICE that LOADALL completed successfully                    - exit ICE mode
     // alujump_names[0x77] = "      ";
        alujump_names[0x78] = "LDBSRM"; // 14    SHRCNT = alu2 & BITS_V;     initialise the barrel shift amount (right) with the value from the register mentioned in the ALU2 source?
        alujump_names[0x79] = "LDBSLM"; // 5     SHLCNT = alu2 & BITS_V;
        alujump_names[0x7A] = "LDBSRU"; // 26    SHRCNT = alu2 & 0x1f;       BT/BTS/BTR/BTC/BSF/PORTIO_PROTCHK/MOV r<->DR/MOV r<->TR/FSAVE/FRSTOR/LSL  initialise the barrel shift amount (right) with the value from the register mentioned in the ALU2 source?
        alujump_names[0x7B] = "LDBSLU"; // 37    SHLCNT = alu2 & 0x1f;
        alujump_names[0x7C] = "RETURN"; // 14
        alujump_names[0x7D] = "ICESIG"; // 10     HLT/FPU_FLAG*/HARDWARE_IRQ/TRIPLE_FAULT*/FAULT*/INTERRUPT/PAGE_FAULT*/SINGLE_STEP*/ICE_PIN*  - send a signal to the ICE that something interesting is happening (*ICE has the opportunity to pause the microcode right after this)
        alujump_names[0x7E] = "PAGEFT"; // 1      PAGE_FAULT  - not sure if this notifies ICE or the paging unit or both or something else that page fault handling has started
        alujump_names[0x7F] = "      "; // 801

     // busop_names[0x00] = "    ";
     // busop_names[0x01] = "    ";
     // busop_names[0x02] = "    ";
     // busop_names[0x03] = "    ";
     // busop_names[0x04] = "rd B";
        busop_names[0x05] = "rd W"; // 1  IRETd_HELPER4
        busop_names[0x06] = "rd  "; // 1  0B1  XCHG rm,r
        busop_names[0x07] = "rd D"; // 9  LSSFSGS_HELPER/LOAD_SEGR_HELP/RETF_HELPER3/LAR_LSL_VERRW2/INT_HELPER      TODO: Figure out the difference between rd and RD  . rd automatically asserts LOCK#? "LOCK# is also asserted automatically for an XCHG instruction, a descriptor update, interrupt acknowledge cycles, and a page table update" (HRM 3.5.1)
        busop_names[0x08] = "CW B"; // 4  FSAVE_CORE/FPU_STORE_CORE                Perform access/limit/paging checks for writes but don't actually do a write
     // busop_names[0x09] = "CW W";
        busop_names[0x0A] = "CW  "; // 4  RCL/RCR rm,CL / INS / ENTER_COMMON
     // busop_names[0x0B] = "CW D";
        busop_names[0x0C] = "CR B"; // 4  FRSTOR_CORE/FPU_LD80_CORE                Perform access/limit/paging checks for reads but don't actually do a read
     // busop_names[0x0D] = "CR W";
     // busop_names[0x0E] = "CR  ";
     // busop_names[0x0F] = "CR D";
     // busop_names[0x10] = "WR B";
        busop_names[0x11] = "WR W"; // 41
        busop_names[0x12] = "WR  "; // 49
        busop_names[0x13] = "WR D"; // 11
     // busop_names[0x14] = "RD B";
        busop_names[0x15] = "RD W"; // 43
        busop_names[0x16] = "RD  "; // 101
        busop_names[0x17] = "RD D"; // 21  LGDT/PR_LD_SS/LLDT/LOADALL386/???
     // busop_names[0x18] = "wr B";
        busop_names[0x19] = "wr W"; // 5     The difference between wr* and WR* seems to be that WR* is used in combination with a copy to OPR_W whereas wr* is for writing back something that was just read, so the data is in OPR_R
        busop_names[0x1A] = "wr  "; // 24
     // busop_names[0x1B] = "wr D";
        busop_names[0x1C] = "PREF"; // 19  Start prefetch at IND?
        busop_names[0x1D] = "IN+D"; // 70  PUSHAd, IRETd         IND += IND_DELTA
     // busop_names[0x1E] = "    ";
        busop_names[0x1F] = "IN+="; // 61  IND = IND + alu2      IND_DELTA = alu2   <-3F-> (0?), -2, -4, 0x0d, 0x5d, 1, 2, 3, 4, 7, 9, INCREM, NEGWSZ, TMPB, TMPC, WORDSZ
     // busop_names[0x20] = "    ";
     // busop_names[0x21] = "    ";
     // busop_names[0x22] = "    ";
     // busop_names[0x23] = "    ";
        busop_names[0x24] = "IN=2"; // 25  IND = alu2                               dest = IND_IO, IND_Rf, IND_TR,        // Theory: {2C}-{39} all select parts/fields of a descriptor cache entry or selector in conjunction with IND_* sources/dests
        busop_names[0x25] = "IND="; // 103 IND = source                             dest = n/a (LJUMP), (-58), INDIDT, INDSTK, INC_CS, IND_ES, IND_IO, IND_IP, IND_OS, IND_RF, IND_TR
        busop_names[0x26] = "IN=+"; // 53  IND = source + alu2   IND_DELTA = alu2   dest = (-4E), (-58-), INDIDT, INDSTK, IND_ES, IND_IP, IND_OS, IND_RF, IND_TR
     // busop_names[0x27] = "    ";
        busop_names[0x28] = "IACK"; // 2   Interrupt Acknowledge bus cycle   HARDWARE_IRQ      (asserts LOCK# like "rd")
     // busop_names[0x29] = "    ";
     // busop_names[0x2A] = "    ";
     // busop_names[0x2B] = "    ";
        busop_names[0x2C] = "SBRM"; // 23  Set BASE in Real Mode (to segment << 4)  dest = DS, ES, FS, GS, IND_CS, IND_DS, IND_ES, IND_FS, IND_GS, IND_SR, IND_SS, IRF, SS      Also set LIMIT to 0xffff? Sets Access Rights?
        busop_names[0x2D] = "SDEH"; // 11                                           dest = (-4D-), INDLDT, IND_CS, IND_SS, IND_TR, IRF       LD_SEGREG_PASS/PRESENT_TSS/PROT_TESTS_PASSED/PROT_TESTS_PASSED2/CALL_UNPRIV/RETF_HELPERRET/LLDT_TEST_PASS/CALL_SAVE_TASK/SWITCH_TASK/PASSED_0B_0D                       set descriptor high dword    (sets byte 3 of base, byte 2 (G/DB/-/A bits) of access, and byte 2 (bits 16-19) of limit)
        busop_names[0x2E] = "SDES"; // 7   LD_SEGREG_PASS/PROT_TESTS_PASSED/PROT_TESTS_PASSED2/CALL_UNPRIV/CALL_SAVE_TASK/SWITCH_TASK                                                                               uses alu2  (TMPB, TMPC, TMPD)                                                                    set descriptor shifted dword (sets byte 3 (P/DPL/S/Type) bits of access, and bytes 0-2 of base)
        busop_names[0x2F] = "SDEL"; // 30  segreg load/JMP_FAR_PM/CALL_FAR_PM/CALL_UNPRIV/TASK_RETURN/IRETd/RETF_HELPER3/LTR/LLDT/LOAD_TASK/TASK_FINAL_V86/TASK_FINAL_PM/TRAPGATE/INTGATE                           uses alu2  (mostly TMPC, one TMPD)                                                               set descriptor low dword     (sets bytes 0-1 of limit)
        busop_names[0x30] = "SPCR"; // 18                                           dest = (-7B-), (-7D-), IRF                                                                                                                 Store Page Cache Register?
        busop_names[0x31] = "SAR "; // 21  select AR field of descriptor cache      dest = ES, FS, GS, IND_CS, IND_ES, IRF, INDLDT, IND_GS, IND_FS, IND_DS, IND_SS, IND_ES, IND_TR, DS
        busop_names[0x32] = "SBAS"; // 9   select BASE field of descriptor cache    dest = DS, (-4D), IRF, IND_TR, IND_RF, IND_IR
        busop_names[0x33] = "SLIM"; // 14  select LIMIT field of descriptor cache   dest = (-4D-), IRF, IND_TR, IND_RF, IND_IP, IND_CS, IND_ES
        busop_names[0x34] = "LPCR"; // 9  MOV rd,CR3/MOV rd,TRn/IRETd_HELPER4/unused?/pla/LOADALL386/DR_ICEBP_HELP2    dest = (-78-), (-79-), (-7A-), (-7B-), IRF               Related to paging?    - store CR3 to IND?      Read special register?  Used with TRn       Load Page Cache Register into IRF2
        busop_names[0x35] = "LAR "; // 11 JMP cp/RETF_HELPER/RETF_HELPER_*/pla/EXCEPTION/LOADALL386/DR_ICEBP_HELP2     dest = (-15-), (-6C-), IND_CS, IND_DS, IND_ES, IND_FS, IND_GS, IND-SS, IND-TR, IRF                      load access rights from descriptor cache into IRF2
        busop_names[0x36] = "LBAS"; // 5  SIDT/SGDT/DR_ICEBP_HELP2                                                     dest = INDGDT, INDIDT, IND_TR, IRF                                                                      load base from descriptor cache into IRF2
        busop_names[0x37] = "LLIM"; // 9  FARJUMP/JMP mp/SIDT/SGDT/SOFTWARE INT/LOADALL386/pla/DR_ICEBP_HELP2          dest = INDGDT, INDIDT, IND_CS, IND_TR, IRF, n/a (LJUMP), (-3F-)                                         load limit from descriptor cache into IRF2
        busop_names[0x38] = "TSDB"; // 3  TASK_RETURN/LOAD_TASK_16B/LOAD_TASK      - task switch debug: some external signal that signifies that the address bus contains the old and new values of TR
        busop_names[0x39] = "SPTR"; // 40  makes DESPTR correspond to dest
     // busop_names[0x3A] = "    ";
     // busop_names[0x3B] = "    ";
        busop_names[0x3C] = "HLTS"; // 1  HLT_HELPER                                dest = (-7F-) - probably outputs HLT/SHUTDOWN signal to motherboard, then the actual waiting is done in the RPT (2) loop at 32b.
     // busop_names[0x3D] = "    ";
     // busop_names[0x3E] = "    ";
        busop_names[0x3F] = "    "; // 1504
        opnames[0] = "RNI"; // 162   RNI must be suppressed if it's in the delay slot - see 3a8
        opnames[1] = "RNi"; // 25    RNI but only if it's executed in the delay slot
        opnames[2] = "RnI"; // 5     RNI but disable interrupts for the next instruction
     // opnames[3] = "   ";
     // opnames[4] = "   ";
        opnames[5] = "FIO"; // 14       FP IO - FSAVE_REGFILE, FRSTOR_REGFILE, FPU_LOAD_CORE, FLDCW, FPU_STORE_CORE, FPU_STORE_287 (when on its own there is a [5] (2) on the line after) - precedes some (but not all) accesses to the FPU port 0x800000fc
        opnames[6] = "RPT"; // 19       AAM/AAD/MUL/IMUL/DIV/IDIV/ARPL/
        opnames[7] = "   "; // 2141
        subopnames[0] = "DLY"; // 559   Wait for bus operation to complete and update OPR_R if it was a read
        subopnames[1] = "UNL"; // 182   Used after a read is complete (but not always immedidately) - to do with bus unlocking?
        subopnames[2] = "WIO"; // 12    Used in HLT, FSAVE_REGFILE, FRSTOR_REGFILE, FPU_LOAD_CORE, FLDCW, FPU_STORE_CORE, FPU_STORE_287  (in conjunction with [5] except in HLT) - wait for IO?
        subopnames[3] = "   "; // 1613

        //                 **************
        testnames[0x00] = "TST_SEL_NONSS";         // test non-SS selector for normal load
        testnames[0x01] = "TST_SEL_CS";            // test CS selector for normal load
        testnames[0x02] = "TST_SEL_RET";           // test selector for RETF
        testnames[0x03] = "TST_SEL_RET_OL";        // test selector for RETF_OUTER_LEV
        testnames[0x04] = "TST_PORTIO_BIT";        // test port IO permission bitmap bits
        testnames[0x05] = "TST_SEL_ARPL";          // test selector for ARPL
        testnames[0x06] = "TST_SEL_GDT";           // test that selector is non-null and points into GDT
        testnames[0x07] = "TST_SEL_LLDT";          // test selector for LLDT
        testnames[0x08] = "TST_SEL_LLVV";          // test selector for LAR/LSL/VERR/VERW
        testnames[0x09] = "TST_SEL_MOREPR";        // test SS selector for MORE_PRIVILEGE
        testnames[0x0A] = "TST_SEL_TASKGT";        // test TSS selector for TASKGATE
        testnames[0x0B] = "TST_SEL_TASKFI";        // test DS/ES/FS/GS selector for TASK_FINAL_PM
        testnames[0x0C] = "TST_SEL_SS";            // test SS selector for normal load (similar to TST_SEL_MOREPR but sets L for non-null selectors)
        testnames[0x0D] = "TST_SEL_TR_TSF";        // test TR selector for TASK_FINAL
     // testnames[0x0E] = "    ";  // unused
     // testnames[0x0F] = "    ";  // unused
        testnames[0x10] = "TST_DES_SIMPLE";        // test descriptor on load (default if not overridden) - descriptor must be present and (conforming code or met the access requirements for non-conforming code)
        testnames[0x11] = "TST_DES_SS";            // test SS descriptor for load
        testnames[0x12] = "TST_DES_JMP";           // test CS descriptor for jump
        testnames[0x13] = "TST_DES_JGATE";         // test CS descriptor for jump gate
        testnames[0x14] = "TST_DES_JGDEST";        // test CS descriptor for jump gate destination
        testnames[0x15] = "TST_DES_CALL";          // test CS descriptor for call
        testnames[0x16] = "TST_DES_CGATE";         // test CS descriptor for call gate
        testnames[0x17] = "TST_DES_CGDEST";        // test CS descriptor for call gate destination
        testnames[0x18] = "TST_DES_RETF";          // test CS descriptor for
        testnames[0x19] = "TST_DES_RTOLSS";        // test SS descriptor for RETF_OUTER_LEV
        testnames[0x1A] = "TST_DES_LDTTSK";        // test LDT descriptor for task switch
        testnames[0x1B] = "TST_DES_LDT";           // test LDT descriptor for LLDT or v86 task switch
        testnames[0x1C] = "TST_DES_MOREPR";        // test stack descriptor for MORE_PRIVILEGE (that is stored in CS)
        testnames[0x1D] = "TST_DES_TSSTSG";        // test TSS descriptor for task gate (needs to be available)
        testnames[0x1E] = "TST_DES_TSSTSR";        // test TSS descriptor for task return (needs to be busy)
        testnames[0x1F] = "TST_DES_TSSLTR";        // test TSS descriptor for LTR (needs to be available and present)
        testnames[0x20] = "TST_DES_GRANUL";        // test granularity of descriptor for LSL
        testnames[0x21] = "TST_DES_INT_SW";        // test IDT entry descriptor for software interrupt (INT ib, INT 3, INTO)
        testnames[0x22] = "TST_DES_INT_HW";        // test IDT entry descriptor for hardware interrutp (IRQ, NMI, fault)
     // testnames[0x23] = "    ";  // unused
        testnames[0x24] = "TST_ACCESS_VIO";        // test DESERR? in ACCESS_VIOLATI handler - this doesn't seem to be a descriptor or a selector. Some kind of error number? (goes from 0-9 and 24-26)
        testnames[0x25] = "TST_DES_RTOLOS";        // test ordinary (DS/ES/FS/GS) descriptor for RETF_OUTER_LEV
        testnames[0x26] = "TST_SEL_LES";           // test selector for LES
        testnames[0x27] = "TST_SEL_LDS";           // test selector for LDS
        testnames[0x28] = "TST_SEL_LFSLGS";        // test selector for LFS/LGS
     // testnames[0x29] = "    ";  // unused
        testnames[0x2A] = "JMP_GFAULT_INT";        // unconditional jump to #GP(I1,E0) with FAULT flag set
        testnames[0x2B] = "READ_RPL";              // PORTIO_PROTCHK, ARPL, LOAD_TASK_COMM - seems like it reads the privilege level (RPL) from a selector (in PROTUN) into a 2-bit latch
        testnames[0x2C] = "WRITE_RPL";             // MORE_PRIVILEGE - seems like it writes the privilege level from the latch to the low two bits of PROTUN
        testnames[0x2D] = "SET_RPL_TO_CPL";        // JUMP_FAR_PM, CALL_FAR_PM, TRAP_INT_GATE - set PROTUN[1..0] = CPL?
        testnames[0x2E] = "COPY_STACK_DPL";        // COPY_PARAMS, IRETd_V86, RETF_OUTER_LEV, TASK_FINAL_V86, TASK_FINAL_PM, FAULT, LOADALL, STOREALL  - copy DPL from PROTUN to special stack descriptor DPL? (DESSTK.DPL = PROTUN[1..0]?)
        testnames[0x2F] = "SET_FAULT";             // signal for the double/triple fault detection logic? (TST_ACCESS_VIO does not set this signal, presumably because the signal from the protection/limit checker is used instead.) Works with the contributory fault bit.
        testnames[0x30] = "TST_DES_LAR";           // test descriptor for LAR
        testnames[0x31] = "TST_DES_LSL";           // test descriptor for LSL
        testnames[0x32] = "TST_DES_VERR";          // test descriptor for VERR
        testnames[0x33] = "TST_DES_VERW";          // test descriptor for VERW
        testnames[0x34] = "FPU_WAIT";
     // testnames[0x35] = "    ";  // unused
     // testnames[0x36] = "    ";  // unused
     // testnames[0x37] = "    ";  // unused
        testnames[0x38] = "FPU_OTHER";
        testnames[0x39] = "FPU_LOAD_3264";
     // testnames[0x3A] = "    ";  // unused
        testnames[0x3B] = "FPU_LOAD_80";
        testnames[0x3C] = "FPU_STORE_3264";
        testnames[0x3D] = "FPU_STORE_80";
        testnames[0x3E] = "FPU_FSAVE";
        testnames[0x3F] = "FPU_FRSTOR";

        auto out = File("microcode_10.txt", true).openWrite();

        String instructions[2560];
        int entryPointer = 0;
        for (int i = 0; i < 2560; ++i) {
            String s;
            while (entryPoints[entryPointer]._u != -1 && entryPoints[entryPointer]._a <= i) {
                if (entryPoints[entryPointer]._a == i) {
                    s = decimal(entryPoints[entryPointer]._u) + " " + /*decimal(entryPoints[entryPointer]._t, 2) + " " +*/ entryPoints[entryPointer]._s;
                    ++entryPointer;
                    break;
                }
                ++entryPointer;
            }
            instructions[i] = s;
        }
        uint64_t uops[2560];
        bool next1DNC = false;
        bool nextDNC = false;
        for (int xx = 0; xx < 2560; ++xx) {
            int x = 2559 - xx;
            //if (instructions[xx] != "")
            if (nextDNC)
                out.write("------------------------------------------|----------------------------------------------------------------------------------------------\n");
            nextDNC = next1DNC;
            next1DNC = false;
            int entryPointer = 0;
            while (entryPoints[entryPointer]._u != -1 && entryPoints[entryPointer]._a <= xx) {
                if (entryPoints[entryPointer]._a == xx) {
                    nextDNC = false;
                    break;
                }
                ++entryPointer;
            }
            int alu2 = 0;
            for (int b = 0; b < 6; ++b)
                if (data[x*37 + 36 - b] == 0xff)  // alu source / jump offset = ABCDEF
                    alu2 += (32 >> b);
            alu2 = 63 - alu2;
            int dst = 0;
            for (int b1 = 0; b1 < 7; ++b1)
                if (data[x*37 + 36 - (b1 + 6)] == 0xff)  // destination = GHIJKLM
                    dst += (64 >> b1);
            dst = 127 - dst;
            int src = 0;
            for (int b2 = 0; b2 < 6; ++b2)
                if (data[x*37 + 36 - (b2 + 13)] == 0xff)  // source = NOPQRS
                    src += (32 >> b2);
            src = 63 - src;
            int alujump = 0;
            for (int b0 = 0; b0 < 7; ++b0)
                if (data[x*37 + 36 - (b0 + 19)] == 0xff)  // alu operation / jump condition = TUVWXYZ
                    alujump += (64 >> b0);
            alujump = 127 - alujump;
            int opcode = 0;
            for (int b = 0; b < 3; ++b)
                if (data[x*37 + 36 - (b + 26)] == 0xff)  // 012
                    opcode += (4 >> b);
            opcode = 7 - opcode;
            int subcode = 0;
            for (int b = 0; b < 2; ++b)
                if (data[x*37 + 36 - (b + 29)] == 0xff)  // 34
                    subcode += (2 >> b);
            subcode = 3 - subcode;
            int buscode = 0;
            for (int b = 0; b < 6; ++b)
                if (data[x*37 + 36 - (b + 31)] == 0xff)  // 56789&
                    buscode += (32 >> b);
            buscode = 63 - buscode;

            String line = hex(xx,3,false) + "  ";
            uint64_t uop = 0;
            for (int i = 0; i < 37; ++i) {
                if (data[x*37 + 36 - i] == 0xff)
                    uop += 1LL << i;
            }
            line += printuop(uop);
            uops[x] = uop;

            out.write(line + "| ");
            if (uop == 0) {
                out.write("                                                      " + instructions[xx]);
                out.write("\n");
                continue;
            }
            if (opcode == 0 || opcode == 2 || alujump == 0x5a || alujump == 0x7c || alujump == 0x71 || alujump == 0x4b)
                next1DNC = true;
            String op = opnames[opcode];
            String subop = subopnames[subcode];
            String busop = busop_names[buscode];

            bool ljump = (alujump >= 0x70 && alujump <= 0x74);
            bool sjump = (alujump >= 0x40 && alujump <= 0x5f);
            bool tjump = (alujump >= 0x60 && alujump <= 0x6f);
            bool haveDest = (dst != 0x7f);
            bool haveNormalDest = ((dst < 0x58 && dst != 0x4d && dst != 0x4e) || dst == 0x5c || dst == 0x5f);
            bool haveSrc = ((src != 0x3f) && !ljump);
            bool haveAlu2 = ((alu2 != 0x3f ) && !(ljump || sjump || tjump));
            bool aluUnary = (alujump == 1 || alujump == 6 || alujump == 0x0b || alujump == 0x14 || alujump == 0x16 || alujump == 0x17 || alujump == 0x1a || alujump == 0x6a);
            bool aluUnary2 = (alujump == 0x15 || alujump == 0x3e || (alujump >= 0x78 && alujump <= 0x7b));
            bool aluBinary = (alujump < 0x20 && !aluUnary && !aluUnary2 && alujump != 0x13);
            bool busBinary = (buscode == 0x26);
            bool busUnary = (buscode == 0x25 || buscode == 0x2c || buscode == 0x2d || (buscode >= 0x31 && buscode <= 0x33));
            bool busUnary2 = (buscode == 0x1f || buscode == 0x24 || buscode == 0x2d || buscode == 0x2e || buscode == 0x2f);
            bool displaySrc = (aluUnary || aluBinary || busUnary || busBinary|| haveNormalDest);
            bool displayDest = haveDest;
            bool displayArrow = haveNormalDest;
            bool displayAlu2 = (aluUnary2 || aluBinary || busUnary2 || busBinary);
            //if (ljump && (displayAlu2 || displaySrc))
            //    out.write("!!!");
            //if (sjump && displayAlu2)
            //    out.write("!!!");
            //if (haveDest && !displayDest)
            //    out.write("!!!");
            //if (haveSrc && !displaySrc && src != 0x2d)  // OPR_R seems to appear on its own sometimes - some kind of bus release?
            //    out.write("!!!");
            //if (haveAlu2 && !displayAlu2)
            //    out.write("!!!");
            if (haveSrc && !displaySrc)  // lines 085, 0b0, 719, 71b, 83c, 86f, 89b and 8ee set source to OPR_R but don't seem to use it. Also 78c sets source to TMP_TR but doesn't seem to use it.
                displaySrc = true;

            if (displaySrc)
                out.write(regnames_source[src]);
            else
                out.write("      ");
            if (displayArrow)
                out.write(" -> ");
            else
                out.write("    ");
            if (displayDest)
                out.write(regnames_dest[dst]);
            else
                out.write("      ");


            //if (!haveDest && (!haveSrc || ljump)) {
            //    if (!ljump && aluBinary)
            //        out.write(String("-1    ") + "    " + "      ");
            //    else
            //        out.write(String("      ") + "    " + "      ");
            //}
            //else {
            //    if (!haveDest)
            //        out.write(regnames_source[src] + "    " + "      ");
            //    else
            //        out.write(regnames_source[src] + " -> " + regnames_dest[dst]);
            //    //if (dst == src)
            //    //    out.write("  " + regnames[dst] + " reflexive");
            //    ++usedD[dst];
            //    ++usedS[src];
            //    lastD[dst] = xx;
            //    lastS[src] = xx;
            //}
            out.write("  ");

            if (ljump) {
                int p = alu2 + (src << 6);
                String s = hex(p, 3, false) + "           ";
                int entryPointer = 0;
                while (entryPoints[entryPointer]._u != -1 && entryPoints[entryPointer]._a <= p) {
                    if (entryPoints[entryPointer]._a == p) {
                        s = entryPoints[entryPointer]._s;
                        if (s.length() < 14)
                            s = s + (14 - s.length())*String(" ");
                        if (s.length() > 14)
                            s = s.subString(0, 14);
                        break;
                    }
                    ++entryPointer;
                }
                out.write(s);
            }
            else {
                if (sjump) {
                    //if (alu2 != 0x3f) {
                    int jump = alu2;
                    int dest = xx + 1 + jump;
                    if (jump > 0x20)
                        dest -= 0x40;
                    String s = hex(dest, 3, false) + "           ";
                    int entryPointer = 0;
                    while (entryPoints[entryPointer]._u != -1 && entryPoints[entryPointer]._a <= dest) {
                        if (entryPoints[entryPointer]._a == dest) {
                            s = entryPoints[entryPointer]._s;
                            if (s.length() < 14)
                                s = s + (14 - s.length())*String(" ");
                            if (s.length() > 14)
                                s = s.subString(0, 14);
                            break;
                        }
                        ++entryPointer;
                    }

                    out.write(s);
                    //}
                    //else
                    //    out.write("              ");
                }
                else {
                    if (tjump) {
                        String s = testnames[alu2];
                        if (s.length() < 14)
                            s = s + (14 - s.length())*String(" ");
                        if (s.length() > 14)
                            s = s.subString(0, 14);
                        out.write(s);
                    }
                    else {
                        //if ((alujump & 0x40) == 0 || (alujump >= 0x60)) {
                        if (displayAlu2) {
                        //if (alu2 != 0x3f || alujump == 0x3e || alujump == 0x15 || (alujump >= 0x0c && alujump <= 0x10) || buscode == 0x1f || buscode == 0x24 || buscode == 0x26 || buscode == 0x2d || buscode == 0x2e) {  // What other alujump commands have alu2==0x3f have a function?
                            //if (regnames_alu[alu2].length() > 6)
                                out.write(regnames_alu[alu2] + String(" ")*(14 - regnames_alu[alu2].length()));
                            //else
                            //    out.write(regnames_alu[alu2] + "-> ALU2 ");
                            //++usedA[alu2];
                            //lastA[alu2] = xx;
                        }
                        else
                            out.write("              ");
                        //}
                    }
                }
            }
            //if ((alu2 + (src << 6)) == 0x5f2 || xx + 1 + alu2 == 0x5f2)
            //    out.write("0x5f2");
            //static const int unreachables[33] = {
            //    0x54d,
            //    0x5b3,
            //    0x5b8,
            //    0x5bd,
            //    0x5d3,
            //    0x686,
            //    0x71f,
            //    0x862,
            //    0x86a,
            //    0x4e7,
            //    0x4ee,
            //    0x554,
            //    0x5d5,
            //    0x5fb,
            //    0x743,
            //    0x7e5,
            //    0x872,
            //    0x8c2,
            //    0x8cb,
            //    0x449,
            //    0x4b5,
            //    0x5da,
            //    0x71c,
            //    0x7e8,
            //    0x3eb,
            //    0x451,
            //    0x51c,
            //    0x536,
            //    0x6a8,
            //    0x6b3,
            //    0x6dd,
            //    0x71a,
            //    0x71d};
            //for (int i = 0; i < 33; ++i) {
            //    int jump = alu2;
            //    if (jump > 0x20)
            //        jump = xx + 1 + jump - 0x40;
            //    else
            //        jump = xx + 1 + jump;

            //    if ((alu2 + (src << 6)) == unreachables[i])
            //        out.write("**" + hex(unreachables[i], 3, true) + "**");
            //    if (jump == unreachables[i])
            //        out.write("**" + hex(unreachables[i], 3, true) + "++");
            //    if ((alu2 + (src << 6)) == unreachables[i] - 1)
            //        out.write("**" + hex(unreachables[i] - 1, 3, true) + "--");
            //    if (jump == unreachables[i] - 1)
            //        out.write("**" + hex(unreachables[i] - 1, 3, true) + "//");
            //}

            ++usedAJ[alujump];
            lastAJ[alujump] = xx;
            ++usedBus[buscode];
            lastBus[buscode] = xx;
            ++usedOp[opcode];
            ++usedSub[subcode];
            out.write(" " + alujump_names[alujump] + "  " + op + " " + subop + " " + busop + " " + instructions[xx]);
#if 0
            //for (int row = 0; row < 368; ++row) {
            //    if ((outputs[row] & 0xfff) == 2559 - x) {
            //        for (int b = 12; b >= 0; --b) {
            //            if ((mask[row] & (1 << b)) == 0)
            //                console.write("?");
            //            else {
            //                if ((bits[row] & (1 << b)) == 0)
            //                    console.write("0");
            //                else
            //                    console.write("1");
            //            }
            //        }
            //        console.write(" ");
            //        for (int b = 15; b >= 0; --b) {
            //            if ((outputs[row] & (1 << b)) == 0)
            //                console.write("0");
            //            else
            //                console.write("1");
            //        }
            //        console.write("  ");
            //    }
            //}
            int nbitsets = 0;
            for (int o = 0; o < 8192; ++o) {
                if ((ored[o] & 0xfff) == 2559 - x) {
                    bitsets[nbitsets]._mask = 0x1fff;
                    bitsets[nbitsets]._bits = o;
                    bitsets[nbitsets]._output = ored[o];
                    ++nbitsets;
                }
            }
            while (true) {
                //if (x == 2559) {
                //    for (int o = 0; o < nbitsets; ++o) {
                //        for (int b = 12; b >= 0; --b) {
                //            if ((bitsets[o]._mask & (1 << b)) == 0)
                //                console.write("?");
                //            else
                //                if ((bitsets[o]._bits & (1 << b)) == 0)
                //                    console.write("0");
                //                else
                //                    console.write("1");
                //        }
                //        console.write(" ");
                //        for (int b = 15; b >= 0; --b) {
                //            if ((bitsets[o]._output & (1 << b)) == 0)
                //                console.write("0");
                //            else
                //                console.write("1");
                //        }
                //        console.write("  ");
                //    }
                //    console.write("\n");
                //}

                bool found = false;
                //for (int o = 0; o < nbitsets; ++o) {
                //    for (int o1 = 0; o1 < o; ++o1) {
                //        if (bitsets[o]._mask != bitsets[o1]._mask || bitsets[o]._output != bitsets[o1]._output)
                //            continue;
                //        Word b = bitsets[o]._bits ^ bitsets[o1]._bits;
                //        if ((b & (b-1)) == 0) {
                //            found = true;
                //            bitsets[o1]._mask &= ~b;
                //            bitsets[o1]._bits &= ~b;
                //            for (int o2 = o; o2 < nbitsets - 1; ++o2) {
                //                bitsets[o2]._mask = bitsets[o2 + 1]._mask;
                //                bitsets[o2]._bits = bitsets[o2 + 1]._bits;
                //                bitsets[o2]._output = bitsets[o2 + 1]._output;
                //            }
                //            --nbitsets;
                //        }
                //    }
                //}
                for (int o = nbitsets - 1; o >= 1; --o) {
                    for (int o1 = o - 1; o1 >= 0; --o1) {
                        if (bitsets[o]._mask != bitsets[o1]._mask || bitsets[o]._output != bitsets[o1]._output)
                            continue;
                        DWord b = bitsets[o]._bits ^ bitsets[o1]._bits;
                        if ((b & (b-1)) == 0) {
                            found = true;
                            bitsets[o1]._mask &= ~b;
                            bitsets[o1]._bits &= ~b;
                            for (int o2 = o; o2 < nbitsets - 1; ++o2) {
                                bitsets[o2]._mask = bitsets[o2 + 1]._mask;
                                bitsets[o2]._bits = bitsets[o2 + 1]._bits;
                                bitsets[o2]._output = bitsets[o2 + 1]._output;
                            }
                            --nbitsets;
                            if (o == nbitsets)
                                break;
                        }
                    }
                }


                if (!found)
                    break;
            }

            for (int o = 0; o < nbitsets; ++o) {
                for (int b = 12; b >= 0; --b) {
                    if ((bitsets[o]._mask & (1 << b)) == 0)
                        console.write("?");
                    else
                        if ((bitsets[o]._bits & (1 << b)) == 0)
                            console.write("0");
                        else
                            console.write("1");
                }
                console.write(" ");
                for (int b = 15; b >= 0; --b) {
                    if ((bitsets[o]._output & (1 << b)) == 0)
                        console.write("0");
                    else
                        console.write("1");
                }
                console.write("  ");
            }
#endif
            out.write("\n");

            //printf("\n");
        }
#if 0
        std::sort(&uops[0], &uops[2560]);
        int first = -1;
        for (int i = 0; i < 2560; ++i) {
            if (i == 2559 || uops[i] != uops[i + 1]) {
                console.write(printuop(uops[i]) + " " + decimal(i - first, 2) + "\n");
                first = i;
            }
        }
#endif

#if 0
        //printf("%i %i %i\n",countr2,countr1,countr0);
        for (int i = 0; i < 64; ++i) {
            if (usedS[i] == 0)
                console.write("     // ");
            else
                console.write("        ");
            console.write("regnames_source["+hex(i, 2)+"] = \"" + regnames_source[i] + "\";");
            if (usedS[i] != 0)
                console.write(" // " + decimal(usedS[i]));
            if (usedS[i] == 1)
                console.write("  " + hex(lastS[i],3, false));
            console.write("\n");

            if (usedD[i] == 0)
                console.write("     // ");
            else
                console.write("        ");
            console.write("regnames_dest  ["+hex(i, 2)+"] = \"" + regnames_dest  [i] + "\";");
            if (usedD[i] != 0)
                console.write(" // " + decimal(usedD[i]));
            if (usedD[i] == 1)
                console.write("  " + hex(lastD[i], 3, false));
            console.write("\n");

            if (usedD[i + 64] == 0)
                console.write("     // ");
            else
                console.write("        ");
            console.write("regnames_dest  ["+hex(i+64, 2)+"] = \"" + regnames_dest[i+64] + "\";");
            if (usedD[i + 64] != 0)
                console.write(" // " + decimal(usedD[i + 64]));
            if (usedD[i + 64] == 1)
                console.write("  " + hex(lastD[i + 64], 3, false));
            console.write("\n");

            if (usedA[i] == 0)
                console.write("     // ");
            else
                console.write("        ");
            console.write("regnames_alu   ["+hex(i, 2)+"] = \"" + regnames_alu   [i] + "\";");
            if (usedA[i] != 0)
                console.write(" // " + decimal(usedA[i]));
            if (usedA[i] == 1)
                console.write("  " + hex(lastA[i], 3, false));
            console.write("\n");
        }
        for (int i = 0; i < 128; ++i) {
            if (usedAJ[i] == 0)
                console.write("     // ");
            else
                console.write("        ");
            console.write("alujump_names["+hex(i, 2)+"] = \"" + alujump_names[i] + "\";");
            if (usedAJ[i] != 0)
                console.write(" // " + decimal(usedAJ[i]));
            if (usedAJ[i] == 1)
                console.write("  " + hex(lastAJ[i], 3, false));
            console.write("\n");
        }
        for (int i = 0; i < 64; ++i) {
            if (usedBus[i] == 0)
                console.write("     // ");
            else
                console.write("        ");
            console.write("busop_names["+hex(i, 2)+"] = \"" + busop_names[i] + "\";");
            if (usedBus[i] != 0)
                console.write(" // " + decimal(usedBus[i]));
            if (usedBus[i] == 1)
                console.write("  " + hex(lastBus[i], 3, false));
            console.write("\n");
        }
        for (int i = 0; i < 8; ++i) {
            if (usedOp[i] == 0)
                console.write("     // ");
            else
                console.write("        ");
            console.write("opnames["+hex(i, 1, false)+"] = \"" + opnames[i] + "\";");
            if (usedOp[i] != 0)
                console.write(" // " + decimal(usedOp[i]));
            console.write("\n");
        }
        for (int i = 0; i < 4; ++i) {
            if (usedSub[i] == 0)
                console.write("     // ");
            else
                console.write("        ");
            console.write("subopnames["+hex(i, 1, false)+"] = \"" + subopnames[i] + "\";");
            if (usedSub[i] != 0)
                console.write(" // " + decimal(usedSub[i]));
            console.write("\n");
        }
        for (int row = 0; row < 368 + 160 + 175; ++row) {
            //for (int b = 12; b >= 0; --b) {
            for (int b = 0; b < 13; ++b) {
                if ((mask[row] & (1 << b)) == 0)
                    console.write("?");
                else {
                    if ((bits[row] & (1 << b)) == 0)
                        console.write("0");
                    else
                        console.write("1");
                }
            }
            console.write(" ");
            for (int b = 15; b >= 0; --b) {
                if ((outputs[row] & (1 << b)) == 0)
                    console.write("0");
                else
                    console.write("1");
            }
            console.write("\n");
        }

        int max = 0;
        for (int x = 0; x < 37 - 5; ++x) {
            Array<int> counts(64);
            for (int y = 0; y < 64; ++y)
                counts[y] = 0;
            for (int u = 0; u < 2560; ++u) {
                int v = 0;
                for (int b = 0; b < 6; ++b) {
                    if (data[u*37 + x + 5 - b] == 0xff)
                        v += (32 >> b);
                }
                ++counts[v];
                if (counts[v] > max && v != 0)
                    max = counts[v];
            }
        }
        Array<Byte> heat(64*(37 - 5));
        for (int x = 0; x < 37 - 5; ++x) {
            Array<int> counts(64);
            for (int y = 0; y < 64; ++y)
                counts[y] = 0;
            for (int u = 0; u < 2560; ++u) {
                int v = 0;
                for (int b = 0; b < 6; ++b) {
                    if (data[u*37 + x + 5 - b] == 0xff)
                        v += (1 << b); //(32 >> b);
                }
                ++counts[v];
            }
            for (int y = 0; y < 64; ++y)
                heat[y + x*64] = clamp(1, counts[y]*255/max, 255);
        }
        File("heat.raw").openWrite().write(heat);

        for (int i = 0; i < 256; ++i) {
            console.write(hex(i,2,false) + ":\n");
            for (int j = 0; j < 8; ++j) {
                int o = (((i & 128) != 0) ? 2048 : 0) +
                        (((i & 64) != 0) ? 1024 : 0) +
                        (((j & 4) != 0) ? 512 : 0) +
                        (((j & 2) != 0) ? 256 : 0) +
                        (((j & 1) != 0) ? 128 : 0) +
                        (((i & 32) != 0) ? 64 : 0) +
                        (((i & 16) != 0) ? 32 : 0) +
                        (((i & 8) != 0) ? 16 : 0) +
                        (((i & 4) != 0) ? 8 : 0) +
                        (((i & 2) != 0) ? 4 : 0) +
                        (((i & 1) != 0) ? 1 : 0);
                console.write(hex(ored[o], 4, false) + "\n");
            }
            console.write("\n");
        }
#endif
    }
};
