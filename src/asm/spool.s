bits 64

%include "macros.inc"
%include "structs.inc"

; spool data memory layout:
;
; 8 [ bit switch or inc/dec count ]
; 8 [ 64-bit pointer to owner	  ]
;
; ... may extend for future functionality

externdef fiber_yield

section .text


define spool_try
	lock bts qword [rdi], 0
	jnc .success

	mov rax, -1
	ret
.success:
	xor rax, rax
	mov [rdi + 8], rsi
	ret
	

define spool_wait
.test:
	xor rax, rax
	mov rcx, 1

	lock cmpxchg [rdi], rcx
	jnz .yield

	mov [rdi + 8], rsi
	ret
; if not free, yield and try next run
.yield:
	pushcaller
	sub rsp, 8
	mov rdi, rsi
	externcall fiber_yield
.then:
	add rsp, 8
	popcaller
	jmp .test


define spool_release
	cmp [rdi + 8], rsi
	je .success

	mov rax, -1
	ret
.success:
	lock btr qword [rdi], 0
	mov qword [rdi + 8], 0
	xor rax, rax
	ret


define spool_bearer
	mov rax, [rdi + 8]
	ret


define spool_inc
.loop:
	mov rax, [rdi]
	mov rcx, rax
	add rcx, 1
	lock cmpxchg [rdi], rcx
	jnz .loop

	mov rax, rcx
	ret


define spool_dec
.loop:
	mov rax, [rdi]
	test rax, rax
	jz .done

	mov rcx, rax
	sub rcx, 1
	lock cmpxchg [rdi], rcx
	jnz .loop

	mov rax, rcx
.done:
	ret
