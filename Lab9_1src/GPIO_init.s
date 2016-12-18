.syntax unified
.cpu cortex-m4
.thumb

.global GPIO_init
	.equ RCC_AHB2ENR,	0x4002104C
	.equ GPIOA_MODER,	0x48000000
	.equ GPIOA_OTYPER,	0x48000004
	.equ GPIOA_OSPEEDR, 0x48000008
	.equ GPIOA_PUPDR, 	0x4800000C
	.equ GPIOA_IDR, 	0x48000010
	.equ GPIOA_ODR, 	0x48000014
	.equ GPIOA_BSRR,	0x48000018
	.equ GPIOA_LCKR, 	0x4800001C
	.equ GPIOA_AFRL, 	0x48000020
	.equ GPIOA_AFRH, 	0x48000024
	.equ GPIOA_BRR, 	0x48000028
	.equ GPIOA_ASCR, 	0x4800002C

	.equ GPIOC_MODER,	0x48000800
	.equ GPIOC_OTYPER,	0x48000804
	.equ GPIOC_OSPEEDR, 0x48000808
	.equ GPIOC_PUPDR, 	0x4800080C
	.equ GPIOC_IDR, 	0x48000810
	.equ GPIOC_ODR, 	0x48000814
	.equ GPIOC_BSRR,	0x48000818
	.equ GPIOC_LCKR, 	0x4800081C
	.equ GPIOC_AFRL, 	0x48000820
	.equ GPIOC_AFRH, 	0x48000824
	.equ GPIOC_BRR, 	0x48000828
	.equ GPIOC_ASCR, 	0x4800082C
	.equ DECODE_MODE, 0x9
	.equ DISPLAY_TEST,0xF
	.equ SCAN_LIMIT, 0xB
	.equ INTENSITY, 0xA
	.equ SHUTDOWN, 0xC
	.equ clock_huze, 10
	.equ one_second_count, 1050000

GPIO_init:
	//TODO: Initialize three GPIO pins as output for max7219 DIN, CS

	//TODO: Initial LED GPIO pins as output 	and CLK
	movs r0, #0x5	//PA PC
	ldr r1, =RCC_AHB2ENR
	str r0,[r1]


	ldr r0, =#0b00000000000000000101010101010101	//input:PA0~7
	LDR r1, =GPIOA_MODER
	ldr r2, [r1]

	ldr r0, =#0b0	//input:PA0~7
	LDR r1, =GPIOA_OTYPER
	ldr r2, [r1]

	ldr r0, =#0b11111111111111110101010101010101	//input:PA0~7
	LDR r1, =GPIOA_PUPDR
	ldr r2, [r1]

	ldr r0, =#0xFFFF
	ldr r1, =GPIOA_OSPEEDR
	strh r0, [r1]




	ldr r0, =#0b01010100000000000000000000000000	//input:PC13~15
	LDR r1, =GPIOC_MODER
	ldr r2, [r1]

	ldr r0, =#0b0	//input:PA0~7
	LDR r1, =GPIOC_OTYPER
	ldr r2, [r1]

	ldr r0, =#0b01010111111111111111111111111111	//input:PA0~7
	LDR r1, =GPIOC_PUPDR
	ldr r2, [r1]

	ldr r0, =#0xFC000000
	ldr r1, =GPIOC_OSPEEDR
	strh r0, [r1]


	BX LR
