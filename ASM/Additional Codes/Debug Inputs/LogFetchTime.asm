# To be inserted at 80376a88

.include "../../Globals.s"

backupall
addi r3, r1, 0x12c # backupall shifts stack by 0x100 and we want struct at 0x2c
rtocbl r12, TM_LogFetchTime
restoreall

# original code line
lbz r0, 2(r31)
