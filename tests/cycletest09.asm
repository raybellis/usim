; cycletest09.asm - MC6809 cycle-count regression fixture
;
; Assembled with: lwasm --6809 --raw -o cycletest09.bin cycletest09.asm
;
; Not a self-checking pass/fail test on its own: tests/cycletest09.cpp drives
; this program and checks the *hardware* cycle count (as handed to an
; attached device, mirroring what a real bus-driven peripheral would see)
; charged to each instruction below against the MC6809 data sheet
; (docs/MC6809.pdf), per NOTES-cycle-counts-for-upstream.md.
;
; Trap pattern: self-branch at the end; the harness detects pc==insn_pc.

zpage		equ	$00
stack_top	equ	$0800

		org	$0400

reset
		lds	#stack_top	; stack init -- cost not checked
		nop			; settle -- cost not checked
		lda	#$00		; LDA immediate  -- expect 2
		lda	<zpage		; LDA direct     -- expect 4
		lda	extdata		; LDA extended   -- expect 5
		ldx	#$1000		; setup -- cost not checked
		ldb	#$08		; setup -- cost not checked
		abx			; ABX            -- expect 3
		bra	skip1		; BRA (taken)    -- expect 3
skip1
		bsr	subr		; BSR            -- expect 7
		jsr	subr		; JSR extended   -- expect 8
		subd	sdata		; SUBD extended  -- expect 7 (already fixed upstream)
		lda	pcr_data,pcr	; LDA n16,PCR    -- expect 9 (already fixed upstream)

trap
		bra	trap

subr
		rts			; RTS            -- expect 5

extdata		fcb	$42
sdata		fdb	$0001
pcr_data	fcb	$a5
