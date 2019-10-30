//
// Created by aings1 on 10/30/19.
//

#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <errno.h>

SYSCALL_DEFINE2(mbx421_init, unsigned int ptrs, unsigned int pro) {
    return 0;
}

SYSCALL_DEFINE0(mbx421_shutdown, void) {
    return 0;
}

SYSCALL_DEFINE1(mbx421_create, unsigned long id) {
    return 0;
}

SYSCALL_DEFINE1(mbx421_destroy, unsigned long id) {
    return 0;
}

SYSCALL_DEFINE1(mbx421_count, unsigned long id) {
    return 0;
}

SYSCALL_DEFINE3(mbx421_send, unsigned long id, const unsigned char __user *msg, long len) {
    return 0;
}

SYSCALL_DEFINE3(mbx421_recv, unsigned long id, unsigned char __user *msg, long len) {
    return 0;
}

SYSCALL_DEFINE3(mbx421_length, unsigned long id) {
    return 0;
}

SYSCALL_DEFINE2(mbx421_acl_add, unsigned long id, pid_t process_id) {
    return 0;
}

SYSCALL_DEFINE2(mbx421_acl_remove, unsigned long id, pid_t process_id) {
    return 0;
}
