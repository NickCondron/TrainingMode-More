# To be inserted at 80375c14

.include "../../Globals.s"

backupall
rtocbl r12, TM_LogScanoutTime
restoreall

# original code line
EXIT:
lwz r0, 0x24(sp)
