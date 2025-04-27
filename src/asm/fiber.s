bits 64

%include "macros.inc"
%include "structs.inc"

section .text

define fiber_run
	push rdi
	mov rbp, rsp

	mov rsi, [rsp + 24]
	mov [rdi + fiber.rip], rsi
	mov rdx, [rdi + fiber.stack]
	add rdx, qword [rdi + fiber.size]
	mov rsp, rdx

	and rsp, -16
	; -16 = -8 (last address) -8 (preserve 16-byte alignment before pseudo-call below)
	sub rsp, 16
	lea rax, [rel .done]
	push rax

	mov rcx, [rdi + fiber.job_proc]
	mov rdi, [rdi + fiber.job_arg]
	jmp rcx
.done:
	mov rsp, rbp
	ret
	
define fiber_yield
	mov rax, [rbp + 8]
	mov rcx, [rsp + 8]
	mov [rax + fiber.rsp], rsp
	mov [rax + fiber.rip], rcx 
.done:
	mov rsp, rbp
	pop rax
	ret

