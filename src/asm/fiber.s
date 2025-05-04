bits 64

%include "macros.inc"
%include "structs.inc"

section .text

define fiber_run	
	; if set, return to where we yielded
	cmp qword [rdi + fiber.rip], 0
	jne .continue

	push rbp
	mov rbp, rsp

	; setup stack
	mov rsp, [rdi + fiber.stack]
	add rsp, [rdi + fiber.size]
	sub rsp, 8
	and rsp, -16
	
	; allocate space for void *arg & fiber pointer
	sub rsp, 32
	mov [rsp + 16], rbp
	mov [rsp + 8], rdi
	mov rcx, [rdi + fiber.job_arg]
	mov [rsp], rcx
	mov [rdi + fiber.meta], rsp

	; set fiber.rip to return address of this routine call
	mov rcx, [rbp + 8]
	mov [rdi + fiber.rip], rcx

	; do job 
	lea rax, [rel .done]
	push rax
	mov rax, rdi

	; pseudo call
	lea rdi, [rsp + 8]
	jmp [rax + fiber.job_proc]
.done:
	mov rsp, rbp
	pop rbp
	ret
.continue:
	; update return addr to latest fiber_run()
	mov rsi, [rdi + fiber.rip]
	mov rdx, [rsp]
	mov [rdi + fiber.rip], rdx
	jmp rsi


define fiber_yield
	mov rsi, [rdi + fiber.rip]    ; fiber_run return addr 
	pushcallee

	lea rdx, [rel .continue]      ; next fiber_run resumes here	    
	mov [rdi + fiber.rip], rdx     
	mov [rdi + fiber.rsp], rsp    ; restore stack later

	mov rdx, [rdi + fiber.meta]
	mov rsp, [rdx + 16]	      ; original stack in meta
	pop rbp

	add rsp, 8		      ; pseudo ret
	jmp rsi 
.continue:
	push rbp
	mov rbp, rsp
	mov rsp, [rdi + fiber.rsp]

	popcallee
	ret


define fiber_self
	mov rax, [rdi + 8]
	ret
	
