bits 64

section .data

struc fiber
	.rip resq 1
	.stack resq 1
endstruc

section .text

global _fiber_get_rip
global _fiber_get_stack

_fiber_get_rip:
	mov rax, [rdi + fiber.rip]
	ret

_fiber_get_stack:
	mov rax, [rdi + fiber.stack]
	ret

