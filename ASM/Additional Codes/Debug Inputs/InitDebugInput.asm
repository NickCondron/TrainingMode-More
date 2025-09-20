# To be inserted at 8016e774

.include "../../Globals.s"

backupall
rtocbl r12, TM_InitDebugInput
restoreall

# original code line
lfs f1, -0x5738(rtoc)
