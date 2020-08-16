#ifndef _EXCEPTION_ASM_H
#define _EXCEPTION_ASM_H

#include "lib.h"
#include "system_call.h"

extern void DE_exception();
extern void DB_exception();
extern void NMI_exception();
extern void BP_exception();
extern void OF_exception();
extern void BR_exception();
extern void UD_exception();
extern void NM_exception();
extern void DF_exception();
extern void CSO_exception();
extern void TS_exception();
extern void NP_exception();
extern void SS_exception();
extern void GP_exception();
extern void PF_exception();
extern void MF_exception();
extern void AC_exception();
extern void MC_exception();
extern void XF_exception();

#endif
