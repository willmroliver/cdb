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
; 8 [ parent fiber (if exists)	]
; 8 [ caller's stack pointer	]
; 8 [ fiber pointer		]
; 8 [ fiber job argument	]
;
; above is in decending order, or ascending from job_arg,
; allowing us to use job_arg as the anchor for finding
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
	; if not READY, return
	test rsi, FIBER_READY & ~FIBER_DONE
	jnz .init
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


; could this routine take a pointer argument to another fiber,
; whose own task we defer to when calling yield on this new task?
; 
; so configure new fiber to 'run' the passed fiber on yield
; - how do we setup fiber.rip & fiber.rsp in this scenario?
; 
; then fiber_yield can be changed to test fiber.rip for non-zero,
; and return early if zero.
define fiber_hijack
	sub rsp, 24
	mov [rsp + 8], rdi

	; dynamically allocate memory for fiber struct as
	; we cannot interfere with the main process stack 
	xor rdi, rdi
	mov rsi, 0x1000
	mov rdx, PROT_READ | PROT_WRITE
	mov r10, MAP_PRIVATE | MAP_ANONYMOUS
	mov r8,	-1
	xor r9, r9

	mov rax, 9
	syscall

	cmp rax, -1
	je .fail

	mov qword [rax + fiber.size], -1 
	mov qword [rax + fiber.flags], FIBER_HIJACKER 
	mov qword [rax + fiber.stack], rsp
	mov qword [rax + fiber.job_proc], 0 
	mov qword [rax + fiber.job_arg], 0

	mov rdi, [rsp + 8]
	test rdi, rdi
	jz .null

	or qword [rax + fiber.flags], FIBER_HIJACKER 

	; configure hijacker meta
	mov [rax + fiber.meta_fiber], rdi ; set parent fiber in meta
	mov [rax + fiber.meta_stack], rbp		  
	mov [rax + fiber.meta_self],  rax ; pointer to self
	mov qword [rax + fiber.meta_arg], 0

	; update parent
	mov [rdi + fiber.meta_fiber], rax	    ; pointer to hijacker
	or qword [rdi + fiber.flags], FIBER_PARENT
	jmp .done
.null:
	mov qword [rax + fiber.rip], 0
	mov qword [rax + fiber.rsp], 0
	jmp .done
.fail:
	xor rax, rax
.done:
	add rsp, 24
	ret

