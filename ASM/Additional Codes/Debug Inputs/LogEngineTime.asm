# To be inserted at 801a4dec 

.include "../../Globals.s"

backupall
rtocbl r12, TM_LogEngineTime
restoreall

# original code line
lwz r0, -0x6C98(r13)
