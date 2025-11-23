# Load immediate test

# .equ TEST 4214
# This could just set a symbol with the "byte_offset" == 4214

fence iO, wori
# addi    x1, %lo(4286)

# Lui:    x1 <- (18<<12)
# Addi:   x1 += -2044