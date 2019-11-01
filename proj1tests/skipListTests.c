//
// Created by aings1 on 10/31/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <sys/syscall.h>

#define sys_mbx421_init 434
#define sys_mbx421_shutdown 435
#define sys_mbx421_create 436
#define sys_mbx421_destroy 437
#define sys_mbx421_count 438
#define sys_mbx421_send 439
#define sys_mbx421_recv 440
#define sys_mbx421_length 441
#define sys_mbx421_acl_add 442
#define sys_mbx421_acl_remove 443
#define sys_mbx421_print 444

/*long init_syscall(unsigned int ptrs, unsigned int prob) {
    return syscall(sys_mbx421_init);
}*/

int main() {
    printf("Hello, World!\n");
    long res = syscall(sys_mbx421_init, 5, 16388);

    unsigned long vals[] = {5, 3, 2, 8, 14, 443, 80, 87, 22, 90, 56, 2};

    printf("-------- Inserting --------\n");
    for (int i  = 0; i < 12; i++) {
        if(syscall(sys_mbx421_create, vals[i]) == 0) {
            printf("Added mailbox id ");
            printf("%lu", vals[i]);
            printf("\n");
        } else {
            printf("Could not add ");
            printf("%lu", vals[i]);
            printf("\n");
        }
    }

    syscall(sys_mbx421_print);

    printf("-------- Mailbox Send Test --------\n");
    const unsigned char testMsg1[5] = {'l','i','s','p'};
    const unsigned char testMsg2[5] = {'b','u','r','p'};
    const unsigned char testMsg3[5] = {'C','M','S','C'};
    printf("Sending %s to mailbox %lu \n", testMsg1, vals[4]);
    syscall(sys_mbx421_send, vals[4], (const unsigned char *) testMsg1, 4);
    printf("Sending %s to mailbox %lu \n", testMsg2, vals[7]);
    syscall(sys_mbx421_send, vals[7], (const unsigned char *) testMsg2, 4);
    printf("Sending %s to mailbox %lu \n", testMsg3, vals[7]);
    syscall(sys_mbx421_send, vals[7], (const unsigned char *) testMsg3, 4);
    printf("Sending %s to mailbox %lu \n", testMsg3, vals[9]);
    syscall(sys_mbx421_send, vals[9], (const unsigned char *) testMsg3, 4);

    syscall(sys_mbx421_print);

    printf("-------- Mailbox Receive Test --------\n");
    unsigned char *testRecv = malloc(4 * sizeof(unsigned char));
    printf("Receiving from mailbox %lu \n", vals[4]);
    syscall(sys_mbx421_recv, vals[4], testRecv, 4);
    printf("Received %s \n", testRecv);
    unsigned char *testRecv2 = malloc(4 * sizeof(unsigned char));
    printf("Receiving from mailbox %lu \n", vals[7]);
    syscall(sys_mbx421_recv, vals[7], testRecv2, 4);
    printf("Received %s \n", testRecv2);
    free(testRecv);
    free(testRecv2);

    syscall(sys_mbx421_print);

    printf("-------- Deleting --------\n");
    for (unsigned long i  = 0; i < 12; i++) {
        if(syscall(sys_mbx421_destroy, vals[i]) == 0) {
            printf("Deleted ");
            printf("%lu", vals[i]);
            printf("\n");
        } else {
            printf("Could not delete ");
            printf("%lu", vals[i]);
            printf("\n");
        }
    }

    syscall(sys_mbx421_print);
    syscall(sys_mbx421_shutdown);
    return 0;
}
