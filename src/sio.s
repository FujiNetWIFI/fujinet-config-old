	;; Call SIO

	.export _siov
	.export _rtclr
	
_siov:	JSR $E459
	RTS
	
_rtclr:	LDA #$00
	STA $12
	STA $13
	STA $14
	RTS
	
