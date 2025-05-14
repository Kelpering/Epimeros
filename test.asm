# Main entrance function
main:
    addi x1, x0, 42       # x1 = 42
    addi x2, x0, 41       # x2 = 41
    beq  x1, x2, yes      # x1 == x2

#   x1 != x2
n~o:
    addi ra, x0, 0
    jal x0, loop

#   x1 == x2
yes:
    addi ra, x0, 1
    jal x0, loop

#   Both run this segment
loop:
    jal x0, loop
