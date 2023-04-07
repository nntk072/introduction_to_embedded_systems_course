
//ARM INSTRUCTIONS
//Instructions are conditional
//32-bit instruction


//How to load data from memory:
//LDR R0, =variable 	(Load address to register. You can use any global variable defined in main.c, or variable found here)
//LDR R1, [R0] 			(Load data from address, in this case 32-bit data)
//LDRB R1, [R0] 		(Load data from address, in this case 8-bit data)
//******
//LDR R0, =0x123456		(Load address to register, Use only if you know the memory address)
//LDR R1, [R0]			(load data from address, in this case 32-bit data)
//LDRB R1, [R0]			(load data from address, in this case 8-bit data)

//How to store data to memory:
//STR R1,[R0]			(R1 contains the data and R0 contains the address where  to store, 32-bit)
//STRB R1,[R0]			(R1 contains the data and R0 contains the address where  to store, 8-bit)


.data
	dir: .byte 1		//8-bit variable for direction
	                    // Add position here
.text
.global blinker
blinker:
	push {r0}				//Push used register to stack 
	push {r1}   
	push {r2}
	push {r3}

	ldr r0, =0x41200000	// load the address of the LED control register into r0
    ldrb r1, [r0]		// load the current LED state into r1
    ldr r2, =dir		// load the address of the direction variable into r2
    ldrb r3, [r2]		// load the current direction into r3

	// turn on the rightmost LED if it's not on
    cmp r1, #0          // compare the current LED state to 0
    bne continue		// branch to "continue" if the LED is already on
    mov r1, #1			// turn on the rightmost LED if it's not on

continue:
    // update the LED state based on the current direction
    cmp r3, #1          // compare the current direction to 1
    beq shift_left		// if moving left, go to shift_left (branch if equal)
    bne shift_right		// if moving right, go to shift_right (branch if not equal)


shift_left:
    lsl r1, r1, #1		// shift the LED state left by one
    cmp r1, #8			// if the 4th LED is on (binary 0b1000), switch direction (8 = 0b1000)
    beq direction_to_right // branch if equal
    b end              // otherwise, go to end

shift_right:
    lsr r1, r1, #1		// shift the LED state right by one
    cmp r1, #1			// if the 1st LED is on (binary 0b001), switch direction
    beq direction_to_left // branch if equal
    b end             // otherwise, go to end

direction_to_right:
    mov r3, #0         // set the direction to 0 (right)
    str r3, [r2]      // store the updated direction
    b end            // go to end

direction_to_left:
    mov r3, #1        // set the direction to 1 (left)
    str r3, [r2]    // store the updated direction
    b end         // go to end

end:
    str r1, [r0]		// store the updated LED state
    bx lr             // return to the place where this function was called
	pop {r3}
	pop {r2}
	pop {r1}
	pop {r0}				//Pop register from stack, Remember the pop order if used more than one register (first pushed, last popped)
	bx lr					//Return to place where this function has been called.






