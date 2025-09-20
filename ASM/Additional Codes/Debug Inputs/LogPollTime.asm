# To be inserted at 80349a28 

.include "../../Globals.s"

backupall
rtocbl r12, TM_LogPollTime
restoreall

# original code line
lwz r5, 0(r24)
