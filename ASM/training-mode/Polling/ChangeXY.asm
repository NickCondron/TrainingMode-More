    # To be inserted at 80402ccc

    .set POLL_COUNT, 10
    .set X, (263 + POLL_COUNT / 2) / POLL_COUNT # division with rounding
    .set Y, POLL_COUNT

    .byte 0x00
    .byte X
    .byte Y
    .byte 0x00
