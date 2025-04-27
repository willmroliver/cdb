bits 64

%include "macros.inc"
%include "structs.inc"

section .text

define fiber_run
	push rbp
	push rdi
	mov rbp, rsp

	mov [rdi + fiber.job_proc], rsi
	mov [rdi + fiber.job_arg], rdx

	mov rsi, [rsp + 24]
	mov [rdi + fiber.rip], rsi
	mov rsp, [rdi + fiber.stack]
	add rsp, [rdi + fiber.size]
	sub rsp, 8
	and rsp, -16
	lea rax, [rel .done]
	push rax

	mov rcx, [rdi + fiber.job_proc]
	mov rdi, [rdi + fiber.job_arg]
	jmp rcx
.done:
	mov rsp, rbp
	pop rdi
	pop rbp
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

