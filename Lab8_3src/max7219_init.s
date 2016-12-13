.syntax unified
.cpu cortex-m4
.thumb
.global max7219_init
	.equ RCC_AHB2ENR,	0x4002104C
	.equ GPIOB_MODER,	0x48000400
	.equ GPIOB_OTYPER,	0x48000404
	.equ GPIOB_OSPEEDR, 	0x48000408
	.equ GPIOB_PUPDR, 	0x4800040C
	.equ GPIOB_ODR, 	0x48000414
	.equ DECODE_MODE, 0x9
	.equ DISPLAY_TEST,0xF
	.equ SCAN_LIMIT, 0xB
	.equ INTENSITY, 0xA
	.equ SHUTDOWN, 0xC


max7219_init:
	//TODO: Initialize max7219 registers
	push {r0, r1, r2, lr}
	ldr r0, =#DECODE_MODE
	ldr r1, =#0xFF	//
	BL max7219_send
	ldr r0, =#DISPLAY_TEST
	ldr r1, =#0x0
	BL max7219_send
	ldr r0, =#SCAN_LIMIT
	ldr r1, =#7	//
	BL max7219_send
	ldr r0, =#INTENSITY
	ldr r1, =#0xA
	BL max7219_send
	ldr r0, =#SHUTDOWN
	ldr r1, =#0x1
	BL max7219_send

	// init all digit to blank
	mov r2, #0 // i
	max7219_init_for:
		add r0, r2, #1
		mov r1, #0x0
		bl max7219_send

		add r2, r2, #1
		cmp r2, #8
		blt max7219_init_for

	pop {r0, r1, r2, pc}
