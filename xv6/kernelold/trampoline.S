	#
        # code to switch between user and kernel space.
        #
        # this code is mapped at the same virtual address
        # (TRAMPOLINE) in user and kernel space so that
        # it continues to work when it switches page tables.
	#
	# kernel.lw causes this to be aligned
        # to a page boundary.
        #

	.section trampsec
.globl trampoline
trampoline:
.align 4
.globl uservec
uservec:    
	#
        # trap.c sets stvec to point here, so
        # traps from user space start here,
        # in supervisor mode, but with a
        # user page table.
        #
        # sscratch points to where the process's p->tf is
        # mapped into user space, at TRAPFRAME.
        #
        
	# swap a0 and sscratch
        # so that a0 is TRAPFRAME
        csrrw a0, sscratch, a0

        # save the user registers in TRAPFRAME
        sw ra, 20(a0)
        sw sp, 24(a0)
        sw gp, 28(a0)
        sw tp, 32(a0)
        sw t0, 36(a0)
        sw t1, 40(a0)
        sw t2, 44(a0)
        sw s0, 48(a0)
        sw s1, 52(a0)
        sw a1, 60(a0)
        sw a2, 64(a0)
        sw a3, 68(a0)
        sw a4, 72(a0)
        sw a5, 76(a0)
        sw a6, 80(a0)
        sw a7, 84(a0)
        sw s2, 88(a0)
        sw s3, 92(a0)
        sw s4, 96(a0)
        sw s5, 100(a0)
        sw s6, 104(a0)
        sw s7, 108(a0)
        sw s8, 112(a0)
        sw s9, 116(a0)
        sw s10, 120(a0)
        sw s11, 124(a0)
        sw t3, 128(a0)
        sw t4, 132(a0)
        sw t5, 136(a0)
        sw t6, 140(a0)

	# save the user a0 in p->tf->a0
        csrr t0, sscratch
        sw t0, 56(a0)

        # restore kernel stack pointer from p->tf->kernel_sp
        lw sp, 4(a0)

        # make tp holw the current hartid, from p->tf->kernel_hartid
        lw tp, 16(a0)

        # load the address of usertrap(), p->tf->kernel_trap
        lw t0, 8(a0)

        # restore kernel page table from p->tf->kernel_satp
        lw t1, 0(a0)
        csrw satp, t1
        sfence.vma zero, zero

        # a0 is no longer valid, since the kernel page
        # table does not specially map p->tf.

        # jump to usertrap(), which does not return
        jr t0

.globl userret
userret:
        # userret(TRAPFRAME, pagetable)
        # switch from kernel to user.
        # usertrapret() calls here.
        # a0: TRAPFRAME, in user page table.
        # a1: user page table, for satp.

        # switch to the user page table.
        csrw satp, a1
        sfence.vma zero, zero

        # put the saved user a0 in sscratch, so we
        # can swap it with our a0 (TRAPFRAME) in the last step.
        lw t0, 56(a0)
        csrw sscratch, t0

        # restore all but a0 from TRAPFRAME
        lw ra, 20(a0)
        lw sp, 24(a0)
        lw gp, 28(a0)
        lw tp, 32(a0)
        lw t0, 36(a0)
        lw t1, 40(a0)
        lw t2, 44(a0)
        lw s0, 48(a0)
        lw s1, 52(a0)
        lw a1, 60(a0)
        lw a2, 64(a0)
        lw a3, 68(a0)
        lw a4, 72(a0)
        lw a5, 76(a0)
        lw a6, 80(a0)
        lw a7, 84(a0)
        lw s2, 88(a0)
        lw s3, 92(a0)
        lw s4, 96(a0)
        lw s5, 100(a0)
        lw s6, 104(a0)
        lw s7, 108(a0)
        lw s8, 112(a0)
        lw s9, 116(a0)
        lw s10, 120(a0)
        lw s11, 124(a0)
        lw t3, 128(a0)
        lw t4, 132(a0)
        lw t5, 136(a0)
        lw t6, 140(a0)

	# restore user a0, and save TRAPFRAME in sscratch
        csrrw a0, sscratch, a0
        
        # return to user mode and user pc.
        # usertrapret() set up sstatus and sepc.
        sret
