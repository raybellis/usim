		swi

		lda	#$80		; enable ACIA interrupt
		sta	acia

loop		cwai	#$bf
		lda	inchar
		jsr	puthexbyte
		swi
		bra	loop
