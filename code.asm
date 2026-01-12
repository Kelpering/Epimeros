# Output is the hex/machine code for the RISC-V CPU
# Output should be assumed to start at address 0.

# Useful tool (NOT MADE BY, NOR INFLUENCED BY ME) for checking RISC-V hex to instruction:
  # https://luplab.gitlab.io/rvcodecjs/

# Examples:

# Basic

  add x1 , x2, x3
label:
  AdD      x1,    x2,x3 
  addi    x1, x2, 1234
  sub     x1, x2, x31

  lui     ra, %hi(label)
  jal     ra, %lo(label)
  fence iow, irow

# Basic Macro
  .macro test 
    addi x1, x2, 123
    addi x1, x2, 123
    addi x1, x2, 123
    addi x1, x2, 123
    addi x1, x2, 123
    addi x1, x2, 123
    addi x1, x2, 123
  .endm

  %test
  addi x1, x2, 312
  %test

# Argument Macro
  .macro args 1
  addi x1, x2, $1
  addi x1, x2, $1
  addi x1, x2, $1
  .endm

  .macro args2 2
  addi x1, x2, $1
  addi x1, x2, $2
  addi x1, x2, $1
  .endm

  %args(2)
  %args2(2, -2048)

  .macro args3 2
  $1 x1, x2, $2
  .endm

  %args3(add, x3)
  %args3(addi, -42)

