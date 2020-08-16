.globl de_asm
.globl db_asm
.globl nmi_asm
.globl bp_asm
.globl of_asm
.globl br_asm
.globl ud_asm
.globl nm_asm
.globl df_asm
.globl cso_asm
.globl ts_asm
.globl np_asm
.globl ss_asm
.globl gp_asm
.globl pf_asm
.globl mf_asm
.globl ac_asm
.globl mc_asm
.globl xf_asm

    /*
    function:       de_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls DE_exception, pops the registers and uses iret
    */
de_asm:
    pushal
    call DE_exception
    popal
    iret

    /*
    function:       db_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls DB_exception, pops the registers and uses iret
    */
db_asm:
    pushal
    call DB_exception
    popal
    iret

    /*
    function:       nmi_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls NMI_exception, pops the registers and uses iret
    */
nmi_asm:
    pushal
    call NMI_exception
    popal
    iret

    /*
    function:       bp_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls BP_exception, pops the registers and uses iret
    */
bp_asm:
    pushal
    call BP_exception
    popal
    iret

    /*
    function:       of_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls OF_exception, pops the registers and uses iret
    */
of_asm:
    pushal
    call OF_exception
    popal
    iret

    /*
    function:       br_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls BR_exception, pops the registers and uses iret
    */
br_asm:
    pushal
    call BR_exception
    popal
    iret

    /*
    function:       ud_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls UD_exception, pops the registers and uses iret
    */
ud_asm:
    pushal
    call UD_exception
    popal
    iret

    /*
    function:       nm_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls NM_exception, pops the registers and uses iret
    */
nm_asm:
    pushal
    call NM_exception
    popal
    iret

    /*
    function:       df_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls DF_exception, pops the registers and uses iret
    */
df_asm:
    pushal
    call DF_exception
    popal
    iret

    /*
    function:       cso_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls CSO_exception, pops the registers and uses iret
    */
cso_asm:
    pushal
    call CSO_exception
    popal
    iret

    /*
    function:       ts_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls TS_exception, pops the registers and uses iret
    */
ts_asm:
    pushal
    call TS_exception
    popal
    iret

    /*
    function:       np_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls NP_exception, pops the registers and uses iret
    */
np_asm:
    pushal
    call NP_exception
    popal
    iret

    /*
    function:       ss_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls SS_exception, pops the registers and uses iret
    */
ss_asm:
    pushal
    call SS_exception
    popal
    iret

    /*
    function:       gp_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls GP_exception, pops the registers and uses iret
    */
gp_asm:
    pushal
    call GP_exception
    popal
    iret

    /*
    function:       pf_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls PF_exception, pops the registers and uses iret
    */
pf_asm:
    pushal
    call PF_exception
    popal
    iret

    /*
    function:       mf_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls MF_exception, pops the registers and uses iret
    */
mf_asm:
    pushal
    call MF_exception
    popal
    iret

    /*
    function:       ac_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls AC_exception, pops the registers and uses iret
    */
ac_asm:
    pushal
    call AC_exception
    popal
    iret

    /*
    function:       mc_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls MC_exception, pops the registers and uses iret
    */
mc_asm:
    pushal
    call MC_exception
    popal
    iret

    /*
    function:       xf_asm
    input:          none
    output:         none
    side effect:    pushes the regisers, calls XF_exception, pops the registers and uses iret
    */
xf_asm:
    pushal
    call XF_exception
    popal
    iret
