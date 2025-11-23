labl:
 
ecall
jal x1, -78912
beq x1, x2, 4
sb x1, x2, 2047
jalr x1, x31, 2047
add x1, x2, x3
    lui x1, %lo(lalbalalbal)
  lui x1, %hi(lalbalalbal)
      # lui x1, %pcrel_hi(labl)
      #   lui x1, %pcrel_lo(labl)


lalbalalbal: AddI x1, x1, 23
# 
