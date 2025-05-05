bits 64

%include "macros.inc"
%include "structs.inc"

%define FIBER_READY	  0x01
%define FIBER_RUNNING	  0x02
%define FIBER_YIELDED	  0x04
%define FIBER_DONE	  0x08
%define FIBER_RECURRING	  0x10

; fiber stack meta memory layout:
;
; 8 [ empty		      ]
; 8 [ caller's stack pointer  ]
; 8 [ fiber pointer	      ]
; 8 [ fiber job argument      ]
;
; above is in decending order, or ascending from job_arg,
; allowing us to use job_arg as the anchor for finding
; the meta (so long as its passed to internal fiber routines).

section .text

define fiber_run
	; if not ready or done, ret
	mov rsi, qword [rdi + fiber.flags]
	mov rdx, rsi
	and rdx, (FIBER_READY) | (FIBER_DONE)
	cmp rdx, FIBER_READY
	je .init
	ret
.init:
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
.run:
	; set fiber.rip to return address of this routine call
	mov rcx, [rbp + 8]
	mov [rdi + fiber.rip], rcx

	; do job 
	lea rax, [rel .done]
	push rax
	mov rax, rdi
	bts qword [rdi + fiber.flags], FIBER_RUNNING

	; pseudo call
	lea rdi, [rsp + 8]
	jmp [rax + fiber.job_proc]
.done:
	; if recurring, re-run
	mov rdi, [rsp + 8]
	bt qword [rdi + fiber.flags], FIBER_RECURRING
	jc .run

	; else mark done and ret
	and qword [rdi + fiber.flags], ~FIBER_RUNNING
	or qword [rdi + fiber.flags], FIBER_DONE

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

	bts qword [rdi + fiber.flags], FIBER_YIELDED

	mov rdx, [rdi + fiber.meta]
	mov rsp, [rdx + 16]	      ; original stack in meta
	pop rbp

	add rsp, 8		      ; pseudo ret
	jmp rsi 
.continue:
	; restore stack and go to the next job instruction 
	push rbp
	mov rbp, rsp
	mov rsp, [rdi + fiber.rsp]

	btc qword [rdi + fiber.flags], FIBER_YIELDED

	popcallee
	ret


define fiber_self
	mov rax, [rdi + 8]
	ret
	
