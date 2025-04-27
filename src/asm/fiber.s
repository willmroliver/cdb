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

	; set fiber.rip to return address of this routine call
	mov rcx, [rbp + 8]
	mov [rdi + fiber.rip], rcx

	; do job 
	lea rax, [rel .done]
	push rax
	mov rax, rdi
	lea rdi, [rsp + 8]
	jmp [rax + fiber.job_proc]
.done:
	mov rsp, rbp
	pop rbp
	ret
.continue:
	jmp [rdi + fiber.rip]


define fiber_yield
	mov rcx, [rdi + 8]
	mov rsi, [rcx + fiber.rip]    ; old rip
	push rsi		      

	lea rdx, [rel .continue]	    
	mov [rcx + fiber.rip], rdx    ; return here later 
	mov [rcx + fiber.rsp], rsp    ; restore stack later

	mov rsp, [rdi + 16]
	pop rbp

	add rsp, 8		      ; pseudo ret
	jmp rsi 
.continue:
	push rbp
	mov rbp, rsp
	mov rsp, [rdi + fiber.rsp]

	pop qword [rdi + fiber.rip]
	ret

