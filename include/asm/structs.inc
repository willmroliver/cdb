struc fiber
	.size	  resq 1
	.flags	  resq 1
	.stack	  resq 1
	.meta	  resq 1
	.rip	  resq 1
	.rsp	  resq 1
	.job_proc resq 1
	.job_arg  resq 1
endstruc

struc spindle
	.fibers	  resq 1
	.idle	  resq 1
	.running  resq 1
	.current  resq 1
endstruc
