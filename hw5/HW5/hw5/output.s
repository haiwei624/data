.text
FA:
# prologue sequence
sw	$ra, 0($sp)
sw	$fp, -4($sp)
add	$fp, $sp, -4
add	$sp, $sp, -8
lw	$2, _framesize_FA
sub	$sp,$sp,$2
sw  $8,72($sp)
sw  $9,68($sp)
sw  $10,64($sp)
sw  $11,60($sp)
sw  $12,56($sp)
sw  $13,52($sp)
sw  $14,48($sp)
sw  $15,44($sp)
sw  $16,40($sp)
sw  $17,36($sp)
sw  $18,32($sp)
sw  $19,28($sp)
sw  $20,24($sp)
sw  $21,20($sp)
sw  $22,16($sp)
sw  $23,12($sp)
sw  $24,8($sp)
sw  $25,4($sp)
_begin_FA:
# prologue sequence end
	 la   $v1, -4($fp)
	 l.s   $f2, 0($v1)
	 la   $v1, -12($fp)
	 s.s   $f2, 0($v1)
	 la   $v1, -12($fp)
	 l.s   $f3, 0($v1)
	 la   $v1, -12($fp)
	 l.s   $f4, 0($v1)
	 mul.s   $f5, $f3, $f4
	 la   $v1, -816($fp)
	 s.s   $f5, 0($v1)
	 la   $v1, -4($fp)
	 l.s   $f6, 0($v1)
	 la   $v1, -816($fp)
	 l.s   $f7, 0($v1)
	 c.le.s   $f6, $f7
	 li   $9, 0
	 bc1t   _1
	 li   $9, 1
_1:
	 beqz   $9, _Lelse1
	 l.s    $f9, _flt1
	 la   $v1, -12($fp)
	 s.s   $f9, 0($v1)
	 j   _Lexit1
_Lelse1:
	 l.s    $f10, _flt2
	 la   $v1, -12($fp)
	 s.s   $f10, 0($v1)
_Lexit1:
	 li    $10, 0
	 la   $v1, -820($fp)
	 sw   $10, 0($v1)
_Wtest1:
	 la   $v1, -820($fp)
	 lw   $11, 0($v1)
	 la   $v1, -4($fp)
	 l.s   $f11, 0($v1)
	 c.lt.s   $f11, $f11
	 li   $12, 1
	 bc1t   _2
	 li   $12, 0
_2:
	 beqz   $12, _Wexit1
	 la   $v1, -12($fp)
	 l.s   $f14, 0($v1)
	 l.s    $f15, _flt3
	 add.s   $f16, $f14, $f15
	 la   $v1, -16($fp)
	 li    $13, 20
	 la   $v1, -820($fp)
	 lw   $14, 0($v1)
	 mul   $15, $13, $14
	 la   $v1, -820($fp)
	 lw   $16, 0($v1)
	 add   $17, $15, $16
	 mul  $17, $17, 4
	 neg  $17, $17
	 add  $v1, $v1, $17
	 s.s   $f16, 0($v1)
	 la   $v1, -820($fp)
	 lw   $18, 0($v1)
	 li    $19, 1
	 add   $20, $18, $19
	 la   $v1, -820($fp)
	 sw   $20, 0($v1)
	 j   _Wtest1
_Wexit1:
	 la   $v1, -816($fp)
	 l.s   $f17, 0($v1)
	 la   $v1, -8($fp)
	 lw   $21, 0($v1)
	 add.s   $f18, $f17, $f21
	 mov.s   $f0, $f18
	 j   _end_FA
# epilogue sequence
_end_FA:
lw  $8,72($sp)
lw  $9,68($sp)
lw  $10,64($sp)
lw  $11,60($sp)
lw  $12,56($sp)
lw  $13,52($sp)
lw  $14,48($sp)
lw  $15,44($sp)
lw  $16,40($sp)
lw  $17,36($sp)
lw  $18,32($sp)
lw  $19,28($sp)
lw  $20,24($sp)
lw  $21,20($sp)
lw  $22,16($sp)
lw  $23,12($sp)
lw  $24,8($sp)
lw  $25,4($sp)
lw	$ra, 4($fp)
add	$sp, $fp, 4
lw	$fp, 0($fp)
jr	$ra
.data
_framesize_FA: .word 1848
_flt1: .float 0.000000
_flt2: .float 1.000000
_flt3: .float 1.000000
# epilogue sequence end
