#Function to compute square root using sqrt.d instruction
        .section        .text,code
        .set            nomips16
        .align  2
        .globl  _sqrtfd

        .ent    _sqrtfd
        .type   _sqrtfd, @function

_sqrtfd:
        .set    noreorder

# Extract sign
         mfhc1   $t7,$f12
         srl     $t8,$t7,31              # dsgn

# Negative argument returns NaN, domain error
         beq     $t8,$zero,10$
         li      $t0,33                  # EDOM
         lui     $v1,0xfff8              # default NaN
         la      $t1,errno
         j       $ra
         sw      $t0,($t1)

# Compute square-root
10$:     sqrt.d  $f0,$f12
         mfc1    $v0,$f0
         j       $ra
         mfhc1   $v1,$f0

         .set    reorder
         .end    _sqrtfd
         .size   _sqrtfd, .-_sqrtfd

