		pshu	a,b,x,y
		pulu	a,b,x,y

		leax	1,x

		lda	#$80		; enable ACIA interrupt
		sta	acia

loop		cwai	#$bf
		lda	inchar
		jsr	puthexbyte
		exg	a, b
		swi
		bra	loop

test		rts
