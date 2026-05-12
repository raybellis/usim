; test6809.asm - MC6809 functional test suite
; Assembled with: asm6809 -B -o test6809.bin test6809.asm
;
; Trap pattern: self-branch; runner checks trap_pc == pass_addr.

; DP scratch (DP=0 throughout)
tmp1		equ	$00
tmp2		equ	$01
tmp3		equ	$02
tmp4		equ	$03
swi_count	equ	$04
swi2_count	equ	$05
swi3_count	equ	$06
nmi_count	equ	$07
; $08: mode flag for SWI dispatcher (0=count mode, 1=rti_e1 test mode)
swi_mode	equ	$08

STACK_TOP	equ	$0400

;----------------------------------------------------------------------
; Reset / entry
;----------------------------------------------------------------------
		org	$0400

reset
		lds	#STACK_TOP
		clra
		tfr	a,dp
		sta	<tmp1
		sta	<tmp2
		sta	<tmp3
		sta	<tmp4
		sta	<swi_count
		sta	<swi2_count
		sta	<swi3_count
		sta	<nmi_count
		sta	<swi_mode

;----------------------------------------------------------------------
; 1. CLR
;----------------------------------------------------------------------
test_clr
		clra
		lbne	fail
		lbmi	fail
		lbcs	fail
		lbvs	fail

		clrb
		lbne	fail
		lbmi	fail

		lda	#$ff
		sta	<tmp1
		clr	<tmp1
		lda	<tmp1
		lbne	fail

;----------------------------------------------------------------------
; 2. COM  (result = ~A; C always 1, V always 0)
;----------------------------------------------------------------------
test_com
		lda	#$00
		coma			; A=$ff, N=1, Z=0, V=0, C=1
		lbcc	fail		; C must be 1 (check before cmpa clobbers it)
		lbvs	fail		; V must be 0
		lbpl	fail		; N must be 1
		cmpa	#$ff
		lbne	fail

		lda	#$ff
		coma			; A=$00, Z=1, C=1
		lbcc	fail		; C must be 1
		lbne	fail		; Z must be 1

;----------------------------------------------------------------------
; 3. NEG  (result = 0 - A)
;----------------------------------------------------------------------
test_neg
		lda	#$00
		nega			; 0-0=0, C=0, Z=1
		lbne	fail
		lbcs	fail		; C=0

		lda	#$01
		nega			; 0-1=$ff, C=1, N=1
		lbcc	fail		; C=1 (check before cmpa)
		lbpl	fail		; N=1
		cmpa	#$ff
		lbne	fail

		lda	#$80
		nega			; 0-$80=$80, C=1, V=1
		lbcc	fail		; C=1 (check before cmpa)
		lbvc	fail		; V=1
		cmpa	#$80
		lbne	fail

;----------------------------------------------------------------------
; 4. ADDA
;----------------------------------------------------------------------
test_add
		lda	#$10
		adda	#$20		; $30, no flags
		cmpa	#$30
		lbne	fail
		lbcs	fail
		lbvs	fail
		lbmi	fail

		lda	#$f0
		adda	#$20		; $10, C=1
		lbcc	fail		; C=1 (check before cmpa)
		cmpa	#$10
		lbne	fail

		lda	#$70
		adda	#$10		; $80, V=1, N=1 (pos+pos=neg)
		lbvc	fail		; V=1 (check before cmpa)
		lbpl	fail		; N=1
		cmpa	#$80
		lbne	fail

		lda	#$80
		adda	#$80		; $00, V=1, C=1, Z=1 (neg+neg=pos)
		lbne	fail
		lbvc	fail
		lbcc	fail

		lda	#$08
		adda	#$08		; $10, H=1
		cmpa	#$10
		lbne	fail
		tfr	cc,b
		bitb	#$20		; H is bit 5 of CC
		lbeq	fail

;----------------------------------------------------------------------
; 5. SUBA
;----------------------------------------------------------------------
test_sub
		lda	#$30
		suba	#$10		; $20, C=0
		cmpa	#$20
		lbne	fail
		lbcs	fail

		lda	#$10
		suba	#$20		; borrow, C=1
		lbcc	fail

		lda	#$7f
		suba	#$ff		; $7f-(-1)=$80, V=1
		lbvc	fail		; V=1 (check before cmpa)
		cmpa	#$80
		lbne	fail

;----------------------------------------------------------------------
; 6. ADCA / SBCA
;----------------------------------------------------------------------
test_adc_sbc
		andcc	#$fe
		lda	#$10
		adca	#$10		; $20 (C was 0)
		cmpa	#$20
		lbne	fail

		orcc	#$01
		lda	#$10
		adca	#$10		; $21 (C was 1)
		cmpa	#$21
		lbne	fail

		andcc	#$fe
		lda	#$20
		sbca	#$10		; $10 (no extra borrow)
		cmpa	#$10
		lbne	fail

		orcc	#$01
		lda	#$20
		sbca	#$10		; $0f (extra borrow)
		cmpa	#$0f
		lbne	fail

;----------------------------------------------------------------------
; 7. Logical: ANDA, ORA, EORA, ANDCC, ORCC
;----------------------------------------------------------------------
test_logical
		lda	#$ff
		anda	#$0f
		cmpa	#$0f
		lbne	fail

		lda	#$00
		ora	#$f0
		lbpl	fail		; N=1 (check before cmpa)
		cmpa	#$f0
		lbne	fail

		lda	#$ff
		eora	#$0f
		cmpa	#$f0
		lbne	fail

		orcc	#$ff
		andcc	#$50		; CC = $50 (F=1, I=1)
		tfr	cc,a
		cmpa	#$50
		lbne	fail

;----------------------------------------------------------------------
; 8. Shifts: ASLA, LSRA, ASRA, ASR mem
;----------------------------------------------------------------------
test_shifts
		lda	#$01
		asla			; $02, C=0
		cmpa	#$02
		lbne	fail
		lbcs	fail

		lda	#$80
		asla			; $00, C=1, Z=1
		lbne	fail
		lbcc	fail

		lda	#$01
		lsra			; $00, C=1, Z=1
		lbne	fail
		lbcc	fail

		lda	#$80
		lsra			; $40, N=0 (zero fill)
		cmpa	#$40
		lbne	fail
		lbmi	fail

		lda	#$80
		asra			; $c0, N=1 (sign extend)
		lbpl	fail		; N=1 (check before cmpa)
		cmpa	#$c0
		lbne	fail

		lda	#$80
		sta	<tmp1
		asr	<tmp1
		lda	<tmp1
		cmpa	#$c0
		lbne	fail

;----------------------------------------------------------------------
; 9. ROL / ROR
;----------------------------------------------------------------------
test_rotate
		andcc	#$fe		; clear C
		lda	#$80
		rola			; A=$00, C=1 (bit7 out), C=0 in
		lbne	fail
		lbcc	fail

		orcc	#$01
		lda	#$00
		rola			; A=$01 (C in), C=0
		cmpa	#$01
		lbne	fail
		lbcs	fail

		orcc	#$01
		lda	#$01
		rora			; A=$80 (C in bit7), C=1 (bit0 out)
		lbcc	fail		; C=1 (check before cmpa)
		cmpa	#$80
		lbne	fail

;----------------------------------------------------------------------
; 10. MUL
;----------------------------------------------------------------------
test_mul
		lda	#$0c
		ldb	#$0a
		mul			; 12*10=120=$0078
		cmpd	#$0078
		lbne	fail

		lda	#$ff
		ldb	#$ff
		mul			; $fe01
		cmpd	#$fe01
		lbne	fail

		lda	#$02
		ldb	#$64		; 2*100=200=$c8
		mul
		lbcc	fail		; C = bit7 of B ($c8 bit7=1) — check before cmpd
		cmpd	#$00c8
		lbne	fail

		lda	#$01
		ldb	#$01
		mul			; D=$0001, B bit7=0, C=0
		lbcs	fail

;----------------------------------------------------------------------
; 11. SEX
;----------------------------------------------------------------------
test_sex
		ldb	#$7f
		sex
		cmpd	#$007f
		lbne	fail
		lbmi	fail

		ldb	#$80
		sex
		lbpl	fail		; N=1 (check before cmpd)
		cmpd	#$ff80
		lbne	fail

;----------------------------------------------------------------------
; 12. ABX  (X = X + B unsigned, flags unaffected)
;----------------------------------------------------------------------
test_abx
		ldx	#$1000
		ldb	#$10
		abx
		cmpx	#$1010
		lbne	fail

		ldx	#$10f0
		ldb	#$20
		abx
		cmpx	#$1110
		lbne	fail

		; Verify CC is not affected: pre-set Z then ABX then verify Z intact
		ldx	#$0100
		ldb	#$01
		orcc	#$04		; set Z
		abx
		tfr	cc,a
		bita	#$04
		lbeq	fail		; Z must still be set

;----------------------------------------------------------------------
; 13. EXG
;----------------------------------------------------------------------
test_exg
		lda	#$12
		ldb	#$34
		exg	a,b
		cmpa	#$34
		lbne	fail
		cmpb	#$12
		lbne	fail

		ldx	#$1234
		ldu	#$5678
		exg	x,u
		cmpx	#$5678
		lbne	fail
		cmpu	#$1234
		lbne	fail

		ldd	#$abcd
		ldx	#$ef01
		exg	d,x
		cmpd	#$ef01
		lbne	fail
		cmpx	#$abcd
		lbne	fail

;----------------------------------------------------------------------
; 14. TFR
;----------------------------------------------------------------------
test_tfr
		lda	#$aa
		tfr	a,b
		cmpb	#$aa
		lbne	fail

		ldx	#$1234
		tfr	x,y
		cmpy	#$1234
		lbne	fail

		lda	#$05		; bits: C=1, Z=1
		tfr	a,cc
		tfr	cc,b
		andb	#$05
		cmpb	#$05
		lbne	fail
		andcc	#$50		; restore sane CC (I=1, F=1)
		orcc	#$04		; set Z

;----------------------------------------------------------------------
; 15. PSHS / PULS / PSHU / PULU
;----------------------------------------------------------------------
test_psh_pul
		lds	#STACK_TOP
		lda	#$11
		ldb	#$22
		ldx	#$3344
		ldy	#$5566
		ldu	#$7788
		pshs	a,b,x,y,u

		clra
		clrb
		ldx	#$0000
		ldy	#$0000
		ldu	#$0000

		puls	a,b,x,y,u
		cmpa	#$11
		lbne	fail
		cmpb	#$22
		lbne	fail
		cmpx	#$3344
		lbne	fail
		cmpy	#$5566
		lbne	fail
		cmpu	#$7788
		lbne	fail

		; PSHU/PULU use U as stack pointer; U=$7788 (valid RAM)
		lda	#$55
		ldb	#$aa
		pshu	a,b
		clra
		clrb
		pulu	a,b
		cmpa	#$55
		lbne	fail
		cmpb	#$aa
		lbne	fail

		lds	#STACK_TOP

;----------------------------------------------------------------------
; 16. Branches — all conditions
;----------------------------------------------------------------------
test_branches
		bra	br_bra_ok
		lbra	fail
br_bra_ok
		lbrn	fail

		lda	#$00
		cmpa	#$00
		beq	br_beq_ok
		lbra	fail
br_beq_ok
		lda	#$01
		cmpa	#$00
		bne	br_bne_ok
		lbra	fail
br_bne_ok

		lda	#$ff
		adda	#$01		; C=1
		bcs	br_bcs_ok
		lbra	fail
br_bcs_ok
		lda	#$01
		adda	#$01		; C=0
		bcc	br_bcc_ok
		lbra	fail
br_bcc_ok

		lda	#$80
		bmi	br_bmi_ok
		lbra	fail
br_bmi_ok
		lda	#$7f
		bpl	br_bpl_ok
		lbra	fail
br_bpl_ok

		lda	#$7f
		adda	#$01		; V=1
		bvs	br_bvs_ok
		lbra	fail
br_bvs_ok
		lda	#$01
		adda	#$01		; V=0
		bvc	br_bvc_ok
		lbra	fail
br_bvc_ok

		lda	#$05
		cmpa	#$03		; unsigned 5>3: C=0, Z=0
		bhi	br_bhi_ok
		lbra	fail
br_bhi_ok
		lda	#$03
		cmpa	#$05		; unsigned 3<5: C=1
		bls	br_bls_ok
		lbra	fail
br_bls_ok

		lda	#$05
		cmpa	#$03		; signed 5>=3: N^V=0
		bge	br_bge_ok
		lbra	fail
br_bge_ok
		lda	#$03
		cmpa	#$05		; signed 3<5: N^V=1
		blt	br_blt_ok
		lbra	fail
br_blt_ok

		lda	#$05
		cmpa	#$03		; signed 5>3
		bgt	br_bgt_ok
		lbra	fail
br_bgt_ok
		lda	#$03
		cmpa	#$05		; signed 3<=5
		ble	br_ble_ok
		lbra	fail
br_ble_ok

;----------------------------------------------------------------------
; 17. Long branches
;----------------------------------------------------------------------
test_lbranch
		lbra	lb_ok
		lbra	fail
lb_ok
		lda	#$00
		cmpa	#$00
		lbeq	lb_beq_ok
		lbra	fail
lb_beq_ok
		lda	#$01
		cmpa	#$00
		lbne	lb_bne_ok
		lbra	fail
lb_bne_ok

;----------------------------------------------------------------------
; 18. BSR / JSR / RTS
;----------------------------------------------------------------------
test_jsr
		bsr	sub_ret42
		cmpa	#$42
		lbne	fail

		ldx	#sub_ret42
		jsr	,x
		cmpa	#$42
		lbne	fail

		lbsr	sub_ret42
		cmpa	#$42
		lbne	fail
		bra	after_sub_ret42

sub_ret42
		lda	#$42
		rts

after_sub_ret42

;----------------------------------------------------------------------
; 19. Indexed addressing modes
;----------------------------------------------------------------------
test_indexed
		ldx	#$0200
		lda	#$99
		sta	2,x
		clra
		lda	2,x
		cmpa	#$99
		lbne	fail

		; ,X+ post-increment (1 byte)
		ldx	#tmp1
		lda	#$ab
		sta	,x+
		cmpx	#tmp2
		lbne	fail
		lda	<tmp1
		cmpa	#$ab
		lbne	fail

		; ,-X pre-decrement (1 byte)
		ldx	#tmp2
		lda	#$cd
		sta	,-x
		cmpx	#tmp1
		lbne	fail
		lda	<tmp1
		cmpa	#$cd
		lbne	fail

		; ,X++ double post-increment (2 bytes)
		ldx	#$0200
		lda	#$11
		sta	,x++
		cmpx	#$0202
		lbne	fail

		; ,--X double pre-decrement (2 bytes)
		ldx	#$0202
		lda	#$22
		sta	,--x
		cmpx	#$0200
		lbne	fail
		lda	$0200
		cmpa	#$22
		lbne	fail

		; B,X accumulator offset
		ldx	#$0200
		ldb	#$04
		lda	#$55
		sta	b,x
		lda	$0204
		cmpa	#$55
		lbne	fail

		; D,X accumulator offset (use load, not store: LDA changes A=D.high)
		lda	#$66
		sta	$0208		; pre-write target
		ldx	#$0200
		lda	#$00
		ldb	#$08		; D=$0008
		lda	d,x		; load from $0200+$0008=$0208
		cmpa	#$66
		lbne	fail

		; Extended indirect [$addr]
		lda	#$02
		sta	$0200
		lda	#$0a
		sta	$0201		; word at $0200 = $020a (pointer)
		lda	#$77
		sta	$020a
		lda	[$0200]
		cmpa	#$77
		lbne	fail

		; PC-relative via LEAX: load a byte defined by a real label
		leax	pcr_data,pcr
		lda	,x
		cmpa	#$a5
		lbne	fail

;----------------------------------------------------------------------
; 20. LEA
;----------------------------------------------------------------------
test_lea
		ldx	#$1000
		leax	$10,x
		cmpx	#$1010
		lbne	fail

		ldy	#$2000
		leay	-1,y
		cmpy	#$1fff
		lbne	fail

		; LEAX clears Z when result = 0
		ldx	#$0000
		leax	0,x
		lbne	fail

		ldx	#$0001
		leax	0,x
		lbeq	fail

		lds	#STACK_TOP
		leas	-4,s
		cmps	#STACK_TOP-4
		lbne	fail
		leas	4,s
		cmps	#STACK_TOP
		lbne	fail

;----------------------------------------------------------------------
; 21. CMP (CMPA, CMPB, CMPD, CMPX, CMPY)
;----------------------------------------------------------------------
test_cmp
		lda	#$50
		cmpa	#$50		; equal: Z=1, C=0
		lbne	fail
		lbcs	fail

		lda	#$50
		cmpa	#$60		; unsigned <: C=1; signed <: BLT holds
		lbcc	fail		; C must be 1
		lbge	fail		; must NOT be signed >=
		lbhi	fail		; must NOT be unsigned >

		ldd	#$1234
		cmpd	#$1234
		lbne	fail

		ldx	#$5678
		cmpx	#$5678
		lbne	fail

		ldy	#$9abc
		cmpy	#$9abc
		lbne	fail

;----------------------------------------------------------------------
; 22. DAA
;----------------------------------------------------------------------
test_daa
		andcc	#$fe
		lda	#$39
		adda	#$27		; binary $60, H=1 (9+7=16)
		daa			; BCD: 39+27=66
		cmpa	#$66
		lbne	fail

		andcc	#$fe
		lda	#$09
		adda	#$09		; binary $12, H=1 (9+9=18)
		daa			; BCD: $18
		cmpa	#$18
		lbne	fail

		andcc	#$fe
		lda	#$99
		adda	#$01		; $9a
		daa			; BCD: 99+1=100, A=$00, C=1
		lbne	fail
		lbcc	fail

;----------------------------------------------------------------------
; 23. SWI / SWI2 / SWI3  (swi_mode=0: count mode)
;----------------------------------------------------------------------
test_swi
		clr	<swi_count
		swi
		lda	<swi_count
		cmpa	#$01
		lbne	fail

		swi
		lda	<swi_count
		cmpa	#$02
		lbne	fail

		clr	<swi2_count
		swi2
		lda	<swi2_count
		cmpa	#$01
		lbne	fail

		clr	<swi3_count
		swi3
		lda	<swi3_count
		cmpa	#$01
		lbne	fail

;----------------------------------------------------------------------
; 24. RTI — E=0 (CC+PC frame only, built via BSR trick)
;----------------------------------------------------------------------
test_rti_e0
		bsr	rti_e0_setup
		; RTI drops control here:
rti_e0_return
		bra	test_rti_e1

rti_e0_setup
		; BSR has pushed PCH,PCL of rti_e0_return onto stack.
		; Push CC=$00 (E=0) on top; then RTI pulls CC+PC.
		lda	#$00
		pshs	a
		rti

;----------------------------------------------------------------------
; 25. RTI — E=1 (full 12-byte frame via SWI; handler modifies saved A/B)
;----------------------------------------------------------------------
test_rti_e1
		; Switch SWI handler to rti_e1 mode
		lda	#$01
		sta	<swi_mode

		lda	#$11		; canary; handler will replace with $aa/$bb
		ldb	#$22
		swi

		clr	<swi_mode	; restore normal SWI mode

		cmpa	#$aa
		lbne	fail
		cmpb	#$bb
		lbne	fail
		bra	pass

; Read-only data used by tests
pcr_data	fcb	$a5		; used by PC-relative LEAX test

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
; Interrupt handlers
;----------------------------------------------------------------------

swi_handler
		; Dispatch on swi_mode flag
		tst	<swi_mode
		bne	swi_rti_e1
		inc	<swi_count
		rti

swi_rti_e1
		; Full E=1 frame on stack:
		; [S+0]=CC, [S+1]=A, [S+2]=B, [S+3]=DP,
		; [S+4..5]=X, [S+6..7]=Y, [S+8..9]=U, [S+10..11]=PC
		lda	#$aa
		sta	1,s
		lda	#$bb
		sta	2,s
		rti

swi2_handler
		inc	<swi2_count
		rti

swi3_handler
		inc	<swi3_count
		rti

irq_handler
		rti

firq_handler
		rti

nmi_handler
		inc	<nmi_count
		rti

;----------------------------------------------------------------------
; Interrupt vectors
;----------------------------------------------------------------------
		org	$fff0
		fdb	$0000
		fdb	swi3_handler
		fdb	swi2_handler
		fdb	firq_handler
		fdb	irq_handler
		fdb	swi_handler
		fdb	nmi_handler
		fdb	reset
