bits 64

%include "macros.inc"
%include "structs.inc"

%define FIBER_READY	  0x01
%define FIBER_RUNNING	  0x02
%define FIBER_YIELDED	  0x04
%define FIBER_DONE	  0x08
%define FIBER_RECURRING	  0x10
%define FIBER_HIJACKER	  0x20
%define FIBER_PARENT	  0x40

%define PROT_READ   0x1
%define PROT_WRITE  0x2

%define MAP_PRIVATE   0x02
%define MAP_ANONYMOUS 0x20

; fiber stack meta memory layout:
;
; 8 [ fiber job argument	]
; 8 [ fiber pointer		]
; 8 [ caller's stack pointer	]
; 8 [ parent fiber (if exists)	]
;
; importantly, it allows us to use job_arg as the anchor for finding
; the meta (so long as it is passed to internal fiber routines).

section .text

define fiber_run
	mov rsi, qword [rdi + fiber.flags]
	test rsi, FIBER_HIJACKER
	jz .ready

	mov rdi, [rdi + fiber.meta_fiber]
	sub rsp, 8
	calldef fiber_yield
	add rsp, 8
	ret
.ready:
	; if not READY, or DONE, return
	and rsi, FIBER_READY | FIBER_DONE
	cmp rsi, FIBER_READY
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

	; configure fiber meta
	mov qword [rdi + fiber.meta_fiber], 0	    
	mov [rdi + fiber.meta_stack], rbp
	mov [rdi + fiber.meta_self], rdi

	; caller's job arg
	mov rcx, [rdi + fiber.job_arg]		
	mov [rdi + fiber.meta_arg], rcx
.run:
	; set fiber.rip to return address of this routine call
	mov rcx, [rbp + 8]
	mov [rdi + fiber.rip], rcx

	push rdi
	sub rsp, 8

	; do job 
	lea rax, [rel .done]
	push rax
	mov rax, rdi
	or qword [rdi + fiber.flags], FIBER_RUNNING

	; pseudo call
	lea rdi, [rax + fiber.meta_arg]
	jmp [rax + fiber.job_proc]
.done:
	; if recurring, re-run
	add rsp, 8
	pop rdi
	mov rsi, [rdi + fiber.flags]
	and rsi, FIBER_RECURRING
	test rsi, rsi
	jnz .run

	; else mark done and ret
	and qword [rdi + fiber.flags], ~FIBER_RUNNING
	or qword [rdi + fiber.flags], FIBER_DONE

	mov rsp, rbp
	pop rbp
	ret
.continue:
	; usually fiber_yield.continue
	mov rsi, [rdi + fiber.rip]

	; update return addr to latest fiber_run()
	mov rdx, [rsp]
	mov [rdi + fiber.rip], rdx
	jmp rsi


define fiber_yield
	; is this the 'hijacker'?	
	mov rsi, [rdi + fiber.flags]
	test rsi, FIBER_HIJACKER
	jz .yield

	; if yielding 'hijacker', simply run 'parent'
	mov rdi, [rdi + fiber.meta_fiber]
	sub rsp, 8
	calldef fiber_run
	add rsp, 8
	ret
.yield:
	mov rsi, [rdi + fiber.rip]    ; fiber_run return addr 
	pushcallee

	lea rdx, [rel .continue]      ; next fiber_run resumes here	    
	mov [rdi + fiber.rip], rdx     
	mov [rdi + fiber.rsp], rsp    ; restore stack later

	or qword [rdi + fiber.flags], FIBER_YIELDED

	mov rsp, [rdi + fiber.meta_stack]
	pop rbp

	add rsp, 8		      ; pseudo ret
	jmp rsi 
.continue:
	; restore stack and go to the next job instruction 
	push rbp
	mov rbp, rsp
	mov rsp, [rdi + fiber.rsp]

	and qword [rdi + fiber.flags], ~FIBER_YIELDED

	popcallee
	ret


define fiber_self
	mov rax, [rdi + 8]
	ret


; we take a pointer argument to another fiber,
; whose own task we defer to when calling yield
;
; this is elegantly handled by redefining 'yield'
; and 'run' on the hijacker thread
;
; yield(hijacker) == run(parent)
; run(hijacker) == yield(parent)
;
; the only real limitation to this approach is
; that fiber_run() can only be called on the 
; hijacker from the parent job_proc.
define fiber_hijack
	mov qword [rdi + fiber.size], -1 
	mov qword [rdi + fiber.flags], FIBER_HIJACKER 
	mov qword [rdi + fiber.stack], rsp
	mov qword [rdi + fiber.job_proc], 0 
	mov qword [rdi + fiber.job_arg], 0
	mov qword [rdi + fiber.meta_arg], 0
	mov qword [rdi + fiber.meta_self], rdi

	test rsi, rsi
	jz .null

	; configure hijacker meta
	mov [rdi + fiber.meta_fiber], rsi ; set parent fiber in meta
	mov [rdi + fiber.meta_stack], rbp		  

	; update parent
	mov [rsi + fiber.meta_fiber], rdi	    ; pointer to hijacker
	or qword [rsi + fiber.flags], FIBER_PARENT
	jmp .done
.null:
	mov qword [rdi + fiber.meta_fiber], 0
	mov qword [rdi + fiber.meta_stack], 0
	mov qword [rdi + fiber.rip], 0
	mov qword [rdi + fiber.rsp], 0
.done:
	ret

