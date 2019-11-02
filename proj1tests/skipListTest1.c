//
// Created by aings1 on 10/31/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <zconf.h>
#include <errno.h>
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
    int size, prob;
    size = 5;
    prob = 16388;
    printf("Initializing with size %d and prob %d \n", size, prob);
    long res = syscall(sys_mbx421_init, size, prob);

    // mailbox ids to test with
    unsigned long vals[] = {5, 3, 2, 8, 14, 443, 80, 87, 22, 90, 56, 2};
    // test adding ids
    printf("\n-------- LIST STATUS IS IN DMESG --------\n\n");
    printf("-------- Inserting --------\n Second 2 should not insert \n");
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
        if (i > 6) {
            // test acl stuff with the last few nodes
            printf("--- ACL ADD TEST ---\n");
            printf("Adding pid of %d to mailbox %lu\n", getpid(), vals[i]);
            long result = syscall(sys_mbx421_acl_add, vals[i], getpid());
            if (result == 0)
                printf("Success\n");
            else
                printf("ERROR CODE %ld\n", result);
        }
    }
    /*printf("--- ACL TEST ---\n");
    printf("Adding pid of %d to mailbox %lu\n", getpid(), 32);
    long result1 = syscall(sys_mbx421_acl_add, 32, getpid());
    if (result1 == 0)
        printf("Success\n");
    else
        printf("ERROR CODE %ld\n", result1);*/

    syscall(sys_mbx421_print);
    // test sending messages
    printf("\n-------- Mailbox Send Test --------\n");
    const unsigned char testMsg1[5] = {'l','i','s','p'};
    const unsigned char testMsg2[5] = {'b','u','r','p'};
    const unsigned char testMsg3[5] = {'C','M','S','C'};
    // unsigned long testBoxes[] = {4, 7, 7, 9};

    /*for (int i = 0; i < 4; i++) {
        long sendTestResult = syscall(sys_mbx421_send, testBoxes[i], (const unsigned char *) testMsg1, 4);
        if (sendTestResult == 0)
            printf("Sending %s to mailbox %lu \n", testMsg1, testBoxes[i]);
        else
            printf("ERROR CODE %ld\n", sendTestResult);
    }*/

    printf("Attempting to send %s to mailbox %lu \n", testMsg1, vals[4]);
    syscall(sys_mbx421_send, vals[4], (const unsigned char *) testMsg1, 4);
    printf("Attempting to send %s to mailbox %lu \n", testMsg2, vals[7]);
    syscall(sys_mbx421_send, vals[7], (const unsigned char *) testMsg2, 4);
    printf("Attempting to send %s to mailbox %lu \n", testMsg3, vals[7]);
    syscall(sys_mbx421_send, vals[7], (const unsigned char *) testMsg3, 4);
    printf("Attempting to send %s to mailbox %lu \n", testMsg3, vals[9]);
    syscall(sys_mbx421_send, vals[9], (const unsigned char *) testMsg3, 4);
    printf("Attempting to send %s to mailbox %lu \n", testMsg1, vals[4]);
    syscall(sys_mbx421_send, vals[4], (const unsigned char *) testMsg1, 4);
    printf("Attempting to send %s to mailbox %lu \n", testMsg2, vals[7]);
    syscall(sys_mbx421_send, vals[7], (const unsigned char *) testMsg2, 4);
    printf("Attempting to send %s to mailbox %lu \n", testMsg3, vals[7]);
    syscall(sys_mbx421_send, vals[7], (const unsigned char *) testMsg3, 4);
    printf("Attempting to send %s to mailbox %lu \n", testMsg3, vals[9]);
    syscall(sys_mbx421_send, vals[9], (const unsigned char *) testMsg3, 4);


    printf("\n-------- Mailbox Length Test --------\n");
    printf("Expect error codes if no permissions\n");
    if (syscall(sys_mbx421_length, vals[4]) == 8)
        printf("Length of first message in mail box %lu is %ld\n", vals[4], syscall(sys_mbx421_length,vals[4]));
    else
        printf("ERROR CODE %ld\n", syscall(sys_mbx421_length, vals[4]));
    if (syscall(sys_mbx421_length, vals[7]) == 8)
        printf("Length of first message in mail box %lu is %ld\n", vals[7], syscall(sys_mbx421_length,vals[7]));
    else
        printf("ERROR CODE %ld\n", syscall(sys_mbx421_length, vals[7]));
    if (syscall(sys_mbx421_length, vals[9]) == 8)
        printf("Length of first message in mail box %lu is %ld\n", vals[9], syscall(sys_mbx421_length,vals[9]));
    else
        printf("ERROR CODE %ld\n", syscall(sys_mbx421_length, vals[9]));


    //printf("Number of messages in mail box %lu is %ld\n", vals[7], syscall(sys_mbx421_count, vals[7]));
    // test count sys call
    printf("\n-------- Mailbox Count Test --------\n");
    if (syscall(sys_mbx421_count, vals[4]) == 2)
        printf("Number of messages in mail box %lu is %ld\n", vals[4], syscall(sys_mbx421_count, vals[4]));
    else
        printf("ERROR CODE %ld\n", syscall(sys_mbx421_length, vals[4]));
    if (syscall(sys_mbx421_count, vals[7]) == 4)
        printf("Number of messages in mail box %lu is %ld\n", vals[7], syscall(sys_mbx421_count, vals[7]));
    else
        printf("ERROR CODE %ld\n", syscall(sys_mbx421_length, vals[7]));
    if (syscall(sys_mbx421_count, vals[9]) == 2)
        printf("Number of messages in mail box %lu is %ld\n", vals[9], syscall(sys_mbx421_count, vals[9]));
    else
        printf("ERROR CODE %ld\n", syscall(sys_mbx421_length, vals[9]));

    vals[9] = 188;
    syscall(sys_mbx421_print);
    // test receive syscall
    printf("\n-------- Mailbox Receive Test --------\n");
    unsigned char *testRecv = malloc(4 * sizeof(unsigned char));
    printf("Attempting to receive from mailbox %lu \n", vals[4]);
    long result = syscall(sys_mbx421_recv, vals[4], testRecv, 4);
    if (result == 0)
        printf("Received %s \n", testRecv);
    else
        printf("ERROR CODE %ld\n", result);
    unsigned char *testRecv2 = malloc(4 * sizeof(unsigned char));
    printf("Attempting to receive from mailbox %lu \n", vals[7]);
    result = syscall(sys_mbx421_recv, vals[7], testRecv2, 4);
    if (result == 0)
        printf("Received %s \n", testRecv2);
    else
        printf("ERROR CODE %ld\n", result);
    free(testRecv);
    free(testRecv2);


    syscall(sys_mbx421_print);
    // test destroy syscall
    printf("\n-------- Deleting --------\n Second 2 & 188 should not delete\n");
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
        if (i > 6) {
            // test acl stuff with the last few nodes
            printf("--- ACL REMOVE TEST ---\n this should error\n");
            printf("Removing pid of %d to mailbox %lu\n", getpid(), vals[i]);
            result = syscall(sys_mbx421_acl_remove, vals[i], getpid());
            if (result == 0)
                printf("Success\n");
            else
                printf("ERROR CODE %ld\n", result);
        }
    }

    syscall(sys_mbx421_print);
    syscall(sys_mbx421_shutdown);
    return 0;
}
