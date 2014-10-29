.data
_globalsym_g1:    .space   4
_globalsym_g2:    .space   4
_globalsym_g3:    .space   4
_globalsym_g4:    .space   4
_globalsym_g5:    .space   8
_globalsym_g6:    .space   8
_globalsym_g7:    .space   8
_globalsym_g8:    .space   8
_globalsym_g9:    .space   8
_globalsym_g10:    .space   8
.text
main:
# prologue sequence
sw	$ra, 0($sp)
sw	$fp, -4($sp)
add	$fp, $sp, -4
add	$sp, $sp, -8
lw	$2, _framesize_main
sub	$sp,$sp,$2
sw  $8,32($sp)
sw  $9,28($sp)
sw  $10,24($sp)
sw  $11,20($sp)
sw  $12,16($sp)
sw  $13,12($sp)
sw  $14,8($sp)
sw  $15,4($sp)
_begin_main:
# prologue sequence end
# epilogue sequence
_end_main:
lw  $8,32($sp)
lw  $9,28($sp)
lw  $10,24($sp)
lw  $11,20($sp)
lw  $12,16($sp)
lw  $13,12($sp)
lw  $14,8($sp)
lw  $15,4($sp)
lw	$ra, 4($fp)
add	$sp, $fp, 4
lw	$fp, 0($fp)
li	$v0, 10
  syscall
.data
framesize_main: .word 1024
# epilogue sequence end
