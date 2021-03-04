		entry % program entry
		align % following instruction align
		addi r1, r0 , topaddr % init stack pointer
		addi r2, r0 , topaddr % init frame pointer
		subi r1, r1, 1148 % set the stack pointer to the top
		addi  r3,  r0, 20
		sw    -1148(r2), r3
		subi r1, r1, 4
		addi r3, r0, 10
		sw -1152(r2), r3
		% start of the loop
		goFor_1		lw    r4, -1152(r2)
		clei  r4, r4, 20
		bz r4, endFor_1
		lw    r5, -1148(r2)
		ceqi  r5, r5, 19
		bz    r5, else_1% if statement
		j endif_1 % jump out of the else block
		else_1		lw    r6, -2296(r2)
		lw    r3, -1152(r2)
		add   r6, r6, r3
		sw    -2296(r2), r6
		endif_1  nop % end of the if statement
		addi  r3, r0, 1
		lw    r5, -1152(r2)
		add   r3, r3, r5
		sw    -1152(r2), r3
		j goFor_1 % jump to start of loop
		endFor_1 nop % end of the loop
		addi r1, r1, 4
		getc  r3
		sw    -1148(r2), r3
		lw    r3, -1148(r2)
		add   r5, r0, r0 % Hold array position
		addi  r5, r5, 20
		addi  r5, r5, 3
		muli  r5, r5, 4
		add   r2, r2, r5
		add   r5, r0, r0 % Hold array position
		addi  r5, r5, 2
		muli  r5, r5, 104
		add   r2, r2, r5
		sw    -1144(r2), r3
		addi r2, r0 , topaddr % reinit frame pointer
		lw    r7, -1148(r2)
		muli  r7, r7, 20
		addi  r5, r0, 5
		muli  r5, r5, 40
		add   r7, r7, r5
		subi  r7, r7, 1
		divi  r7, r7, 2
		sw    -1148(r2), r7
		lw    r5, -1148(r2)
		add   r6, r0, r0 % Hold array position
		addi  r6, r6, 20
		addi  r6, r6, 3
		muli  r6, r6, 4
		add   r2, r2, r6
		add   r6, r0, r0 % Hold array position
		addi  r6, r6, 2
		muli  r6, r6, 104
		add   r2, r2, r6
		lw    r3, -1144(r2)
		addi r2, r0 , topaddr % reinit frame pointer
		add   r5, r5, r3
		putc  r5
		lw    r3, -1148(r2)
		putc  r3
		hlt % close program
