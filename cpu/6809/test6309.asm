; test6309.asm - HD6309 functional test suite
; Assembled with: asm6809 --6309 -B -o test6309.bin test6309.asm
;
; Exercises the 6309-specific additions on top of the 6809:
;   - New accumulators E, F, W, Q
;   - LD/ST/ADD/SUB/CMP/INC/DEC/NEG/COM/TST/CLR/shift/rotate on the new regs
;   - SEXW, PSHSW/PULSW/PSHUW/PULUW
;   - TFR/EXG with new register encodings and size-mismatched transfers
;   - AIM/OIM/EIM/TIM in-memory bit logic
;   - MULD, DIVD, DIVQ (including divide-by-zero trap through $FFF0)
;   - TFM block transfer
;   - LDMD / BITMD
;
; Trap pattern: self-branch; runner checks trap_pc == pass_addr.

; DP scratch (DP=0 throughout)
tmp1		equ	$00
tmp2		equ	$01
tmp3		equ	$02
tmp4		equ	$03
tmp5		equ	$04
tmp6		equ	$05
tmp7		equ	$06
tmp8		equ	$07
trap_flag	equ	$08		; set non-zero by the trap handler
trap_cause	equ	$09		; 1 = DZ trap, 2 = IL trap

STACK_TOP	equ	$0400

;----------------------------------------------------------------------
; Reset / entry
;----------------------------------------------------------------------
		org	$0400

reset
		lds	#STACK_TOP
		clra
		tfr	a,dp
		clr	<trap_flag
		clr	<trap_cause

;----------------------------------------------------------------------
; LDE / STE
;----------------------------------------------------------------------
test_lde_ste
		lde	#$5a
		ste	<tmp1
		lda	<tmp1
		cmpa	#$5a
		lbne	fail
		lde	#$00
		lbne	fail			; Z=1 after LDE #0
		lde	#$80
		lbpl	fail			; N=1 after LDE #$80

;----------------------------------------------------------------------
; LDF / STF
;----------------------------------------------------------------------
test_ldf_stf
		ldf	#$a5
		stf	<tmp2
		lda	<tmp2
		cmpa	#$a5
		lbne	fail
		ldf	#$00
		lbne	fail
		ldf	#$80
		lbpl	fail

;----------------------------------------------------------------------
; LDW / STW
;----------------------------------------------------------------------
test_ldw_stw
		ldw	#$1234
		stw	<tmp1
		ldd	<tmp1
		cmpd	#$1234
		lbne	fail
		ldw	#$0000
		lbne	fail
		ldw	#$8000
		lbpl	fail

;----------------------------------------------------------------------
; LDQ / STQ
;----------------------------------------------------------------------
test_ldq_stq
		ldq	#$12345678
		stq	<tmp1
		ldd	<tmp1
		cmpd	#$1234
		lbne	fail
		ldd	<tmp3
		cmpd	#$5678
		lbne	fail
		ldq	#$00000000
		lbne	fail
		ldq	#$80000000
		lbpl	fail

;----------------------------------------------------------------------
; ADDE / SUBE / CMPE
;----------------------------------------------------------------------
test_adde_sube_cmpe
		lde	#$10
		adde	#$20
		cmpe	#$30
		lbne	fail
		lde	#$ff
		adde	#$01			; result $00, C=1, Z=1
		lbne	fail			; Z=1
		lbcc	fail			; C=1
		lde	#$30
		sube	#$10
		cmpe	#$20
		lbne	fail
		lde	#$42
		cmpe	#$42
		lbne	fail
		lbcs	fail			; C=0 when equal

;----------------------------------------------------------------------
; ADDF / SUBF / CMPF
;----------------------------------------------------------------------
test_addf_subf_cmpf
		ldf	#$05
		addf	#$03
		cmpf	#$08
		lbne	fail
		ldf	#$10
		subf	#$05
		cmpf	#$0b
		lbne	fail
		ldf	#$77
		cmpf	#$77
		lbne	fail

;----------------------------------------------------------------------
; ADDW / SUBW / CMPW
;----------------------------------------------------------------------
test_addw_subw_cmpw
		ldw	#$1234
		addw	#$1111
		cmpw	#$2345
		lbne	fail
		ldw	#$2000
		subw	#$0fff
		cmpw	#$1001
		lbne	fail
		ldw	#$abcd
		cmpw	#$abcd
		lbne	fail

;----------------------------------------------------------------------
; INC / DEC on D, E, F, W
;----------------------------------------------------------------------
test_inc_dec
		lde	#$7f
		ince				; E=$80, V=1, N=1
		lbvc	fail
		cmpe	#$80
		lbne	fail
		lde	#$80
		dece				; E=$7f, V=1
		lbvc	fail
		cmpe	#$7f
		lbne	fail

		ldf	#$ff
		incf				; F=$00, Z=1
		lbne	fail
		cmpf	#$00
		lbne	fail

		ldd	#$7fff
		incd				; D=$8000, V=1
		lbvc	fail
		cmpd	#$8000
		lbne	fail

		ldw	#$0001
		decw				; W=$0000, Z=1
		lbne	fail
		cmpw	#$0000
		lbne	fail

;----------------------------------------------------------------------
; NEGD
;----------------------------------------------------------------------
test_negd
		ldd	#$0001
		negd				; D=$ffff, C=1, N=1
		lbcc	fail			; C must be 1 (check before cmpd clobbers it)
		lbpl	fail			; N must be 1
		cmpd	#$ffff
		lbne	fail
		ldd	#$8000
		negd				; D=$8000, V=1 (overflow case)
		lbvc	fail
		cmpd	#$8000
		lbne	fail

;----------------------------------------------------------------------
; COM on D, E, F, W (C always 1, V always 0)
;----------------------------------------------------------------------
test_com
		ldd	#$1234
		comd				; D=$edcb
		lbcc	fail
		lbvs	fail
		cmpd	#$edcb
		lbne	fail

		lde	#$00
		come				; E=$ff
		lbpl	fail			; N must be 1 (check before cmpe)
		cmpe	#$ff
		lbne	fail
		ldf	#$ff
		comf				; F=$00
		lbne	fail			; Z must be 1
		cmpf	#$00
		lbne	fail

		ldw	#$0f0f
		comw				; W=$f0f0
		cmpw	#$f0f0
		lbne	fail

;----------------------------------------------------------------------
; CLR on D, E, F, W
;----------------------------------------------------------------------
test_clr
		ldd	#$ffff
		clrd
		lbne	fail			; Z must be 1 (check before cmpd)
		lbcs	fail			; C must be 0
		cmpd	#$0000
		lbne	fail

		lde	#$ff
		clre
		cmpe	#$00
		lbne	fail
		ldf	#$ff
		clrf
		cmpf	#$00
		lbne	fail
		ldw	#$ffff
		clrw
		cmpw	#$0000
		lbne	fail

;----------------------------------------------------------------------
; SEXW: sign-extend W into Q
;----------------------------------------------------------------------
test_sexw
		ldw	#$8000
		sexw				; D <- $ffff
		cmpd	#$ffff
		lbne	fail
		cmpw	#$8000			; W unchanged
		lbne	fail

		ldw	#$1234
		sexw				; D <- $0000
		cmpd	#$0000
		lbne	fail
		cmpw	#$1234
		lbne	fail

		ldw	#$0000
		sexw
		lbne	fail			; Z=1 (Q==0)

;----------------------------------------------------------------------
; Shifts and rotates on D and W (representative subset)
;----------------------------------------------------------------------
test_shifts
		ldd	#$8001
		lsrd				; D=$4000, C=1 (from bit 0)
		lbcc	fail			; C must be 1 (check before cmpd)
		cmpd	#$4000
		lbne	fail

		ldd	#$8001
		asrd				; D=$c000, C=1 (sign-extending)
		lbcc	fail
		cmpd	#$c000
		lbne	fail

		ldd	#$4001
		lsld				; D=$8002, V=1 (bit15 xor bit14)
		lbvc	fail
		cmpd	#$8002
		lbne	fail

		ldw	#$1234
		lsrw
		cmpw	#$091a
		lbne	fail

;----------------------------------------------------------------------
; PSHSW / PULSW round-trip
;----------------------------------------------------------------------
test_psh_pul_w
		ldw	#$abcd
		pshsw
		ldw	#$0000
		pulsw
		cmpw	#$abcd
		lbne	fail

;----------------------------------------------------------------------
; TFR with new registers
;----------------------------------------------------------------------
test_tfr_new
		ldw	#$1234
		tfr	w,d
		cmpd	#$1234
		lbne	fail
		ldd	#$5678
		tfr	d,w
		cmpw	#$5678
		lbne	fail

;----------------------------------------------------------------------
; TFR with the 0 register (codes 12/13, byte position)
;----------------------------------------------------------------------
test_tfr_zero
		lda	#$ff
		tfr	0,a			; A <- 0
		cmpa	#$00
		lbne	fail

;----------------------------------------------------------------------
; TFR with size mismatch (8 -> 16 fills high byte with $FF;
; 16 -> 8 takes low byte)
;----------------------------------------------------------------------
test_tfr_mismatch
		lda	#$5a
		tfr	a,x
		cmpx	#$ff5a
		lbne	fail

		ldx	#$1234
		tfr	x,a
		cmpa	#$34
		lbne	fail

;----------------------------------------------------------------------
; EXG with new registers
;----------------------------------------------------------------------
test_exg_new
		ldw	#$1111
		ldd	#$2222
		exg	w,d
		cmpw	#$2222
		lbne	fail
		cmpd	#$1111
		lbne	fail

;----------------------------------------------------------------------
; AIM / OIM / EIM / TIM (direct mode)
;----------------------------------------------------------------------
test_aim_dir
		lda	#$ff
		sta	<tmp1
		aim	#$0f,<tmp1		; tmp1 = $0f
		lda	<tmp1
		cmpa	#$0f
		lbne	fail

test_oim_dir
		lda	#$00
		sta	<tmp1
		oim	#$5a,<tmp1		; tmp1 = $5a
		lda	<tmp1
		cmpa	#$5a
		lbne	fail

test_eim_dir
		lda	#$ff
		sta	<tmp1
		eim	#$0f,<tmp1		; tmp1 = $f0
		lda	<tmp1
		cmpa	#$f0
		lbne	fail

test_tim_dir
		lda	#$5a
		sta	<tmp1
		tim	#$0f,<tmp1		; $5a & $0f = $0a, Z=0
		lbeq	fail
		tim	#$a5,<tmp1		; $5a & $a5 = $00, Z=1
		lbne	fail

;----------------------------------------------------------------------
; MULD: signed 16x16 -> 32 in Q
;----------------------------------------------------------------------
test_muld
		ldd	#$0010			; 16
		muld	#$0020			; 32
		cmpd	#$0000			; high word of 512 is 0
		lbne	fail
		cmpw	#$0200			; low word is 0x200 = 512
		lbne	fail

		ldd	#$ffff			; -1 signed
		muld	#$0001			; -1 * 1 = -1
		cmpd	#$ffff
		lbne	fail
		cmpw	#$ffff
		lbne	fail

;----------------------------------------------------------------------
; DIVD: 16/8 -> 8 quotient in B, 8 remainder in A
;----------------------------------------------------------------------
test_divd
		ldd	#$0064			; 100
		divd	#$0a			; 100 / 10 = 10 r 0
		cmpa	#$00
		lbne	fail
		cmpb	#$0a
		lbne	fail

;----------------------------------------------------------------------
; DIVQ: 32/16 -> 16 quotient in W, 16 remainder in D
;----------------------------------------------------------------------
test_divq
		ldq	#$0000ffff		; 65535
		divq	#$0010			; 65535/16 = 4095 r 15
		cmpw	#$0fff
		lbne	fail
		cmpd	#$000f
		lbne	fail

;----------------------------------------------------------------------
; TFM r+,r+: byte block copy
; Source at src_block, destination at dst_block, length 4.
;----------------------------------------------------------------------
test_tfm
		ldx	#src_block
		ldy	#dst_block
		ldw	#4
		tfm	x+,y+

		; After TFM: X = src_block+4, Y = dst_block+4, W = 0
		ldd	dst_block
		cmpd	#$dead
		lbne	fail
		ldd	dst_block+2
		cmpd	#$beef
		lbne	fail

;----------------------------------------------------------------------
; LDMD / BITMD
;----------------------------------------------------------------------
test_ldmd_bitmd
		ldmd	#$03			; set NM=1, FM=1
		bitmd	#$01			; test NM bit
		lbeq	fail			; should be set
		bitmd	#$02			; test FM bit
		lbeq	fail			; should be set
		ldmd	#$00			; clear NM, FM (back to emulation)
		bitmd	#$01
		lbne	fail			; should be clear now

;----------------------------------------------------------------------
; Divide-by-zero trap through $FFF0
;----------------------------------------------------------------------
test_divd_zero_trap
		clr	<trap_flag
		clr	<trap_cause
		ldd	#$0100
		divd	#$00			; divisor 0 -> trap
		; Resume after trap; handler set trap_flag and trap_cause
		lda	<trap_flag
		lbeq	fail
		lda	<trap_cause
		cmpa	#1			; 1 = DZ trap
		lbne	fail

;----------------------------------------------------------------------
; ALL TESTS PASSED
;----------------------------------------------------------------------
pass
		bra	pass

;----------------------------------------------------------------------
; Failure trap
;----------------------------------------------------------------------
fail
		lbra	fail

;----------------------------------------------------------------------
; Block data for TFM test
;----------------------------------------------------------------------
src_block
		fcb	$de,$ad,$be,$ef
dst_block
		fcb	$00,$00,$00,$00

;----------------------------------------------------------------------
; Trap handler for illegal-instruction / divide-by-zero through $FFF0.
; BITMD with bit-7 mask checks DZ (and clears it); bit-6 checks IL.
;----------------------------------------------------------------------
trap_handler
		inc	<trap_flag
		bitmd	#$80			; test DZ (and clear it)
		bne	trap_dz
		bitmd	#$40			; test IL (and clear it)
		bne	trap_il
		rti				; spurious; leave cause as 0
trap_dz
		lda	#1
		sta	<trap_cause
		rti
trap_il
		lda	#2
		sta	<trap_cause
		rti

;----------------------------------------------------------------------
; Interrupt handlers (RTI for unused vectors)
;----------------------------------------------------------------------
irq_handler	rti
firq_handler	rti
nmi_handler	rti
swi_handler	rti
swi2_handler	rti
swi3_handler	rti

;----------------------------------------------------------------------
; Interrupt vectors
;----------------------------------------------------------------------
		org	$fff0
		fdb	trap_handler		; $fff0 reserved -> 6309 trap
		fdb	swi3_handler
		fdb	swi2_handler
		fdb	firq_handler
		fdb	irq_handler
		fdb	swi_handler
		fdb	nmi_handler
		fdb	$0400			; reset vector -> entry
