global _sum

segment .text

_sum:
  push rbx
  push rbp

  mov rax, rdi
  add rax, rsi

  pop rbp
  pop rbx

  ret

