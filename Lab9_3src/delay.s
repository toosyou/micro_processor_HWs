.syntax unified
.cpu cortex-m4
.thumb

.text
	.global delay


delay:
	MOV r1, #1
	MOV r1, #2
	MOV r1, #3
	MOV r1, #4
	MOV r1, #5
	SUB r0, #1
	CMP r0, #0
	BNE delay
	BX LR
