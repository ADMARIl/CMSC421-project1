//
// Created by aings1 on 11/1/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
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

int main() {
    printf("Hello, World!\n");
    long res = syscall(sys_mbx421_init, 5, 16388);

    // mailbox ids to test with
    unsigned long vals[] = {5, 3, 2, 8, 14, 443, 80, 87, 22, 90, 56, 2};

    pid_t pid;
    // fork proccess
    pid = fork();

    // Check if fork failed
    if (pid < 0) {
        fprintf(stderr, "Fork Failed");
        return 1;
    } else if (pid == 0) {
        // Child Proccess

        // execute user args
        // if exec is unsuccessful, prompt user with error and kill child
        if (execvp(finalArgs[0],finalArgs) != 0) {
            printf( " %s\n", "Unknown Command. " );
            return 0;
        }
        // exit child if we need to exit
        if (exit == 0) {
            return 0;
        }
    } else {
        // parent proccess

        // wait for child to complete
        wait(NULL);
    }

    return 0;
}