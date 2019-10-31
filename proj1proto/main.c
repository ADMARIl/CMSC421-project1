/*
 *  File: main.c
 *  Author: Andrew Ingson (aings1@umbc.edu)
 *  Date; 10/13/19
 *  CMSC 421 (Principles of Operating Systems) Project 1
 *
 *  Driver for testing the skipList functions
 */

#include <stdio.h>
#include <time.h>
#include "skipList.h"

unsigned int SKIPLIST_SIZE = 5;

int main() {
    // generate probability
    seed_random(time(NULL));
    //seed_random(1000000);
    // test vals
    unsigned long vals[] = {5, 3, 2, 8, 14, 443, 80, 87, 22, 90, 56, 66};

    // start skipList
    // struct skipList_head skipList;
    skipList_initialize(SKIPLIST_SIZE, 16383);

    // add to skipList
    printf("-------- Inserting --------\n");
    for (int i  = 0; i < 12; i++) {
        if(skipList_add(vals[i]) == 0) {
            printf("Added mailbox id ");
            printf("%lu", vals[i]);
            printf("\n");
        } else {
            printf("Could not add ");
            printf("%lu", vals[i]);
            printf("\n");
        }
    }

    printf("-------- Mailbox Send Test --------\n");
    const unsigned char testMsg1[5] = {'l','i','s','p'};
    const unsigned char testMsg2[5] = {'b','u','r','p'};
    const unsigned char testMsg3[5] = {'C','M','S','C'};
    printf("Sending %s to mailbox %lu \n", testMsg1, vals[4]);
    mBox_send(vals[4], (const unsigned char *) testMsg1, 4);
    printf("Sending %s to mailbox %lu \n", testMsg2, vals[7]);
    mBox_send(vals[7], (const unsigned char *) testMsg2, 4);
    printf("Sending %s to mailbox %lu \n", testMsg3, vals[7]);
    mBox_send(vals[7], (const unsigned char *) testMsg3, 4);
    printf("Sending %s to mailbox %lu \n", testMsg3, vals[9]);
    mBox_send(vals[9], (const unsigned char *) testMsg3, 4);

    skipList_print();

    printf("-------- Search for each value --------\n");
    vals[9] = 188;
    for (int i  = 0; i < 12; i++) {
        if(skipList_search(vals[i]) == 0) {
            printf("Found ");
            printf("%lu", vals[i]);
            printf("\n");
        } else {
            printf("Could not find ");
            printf("%lu", vals[i]);
            printf("\n");
        }
    }

    printf("-------- Mailbox Receive Test --------\n");
    unsigned char *testRecv = malloc(4 * sizeof(unsigned char));
    printf("Receiving from mailbox %lu \n", vals[4]);
    mBox_recv(vals[4], testRecv, 4);
    printf("Received %s \n", testRecv);
    unsigned char *testRecv2 = malloc(4 * sizeof(unsigned char));
    printf("Receiving from mailbox %lu \n", vals[7]);
    mBox_recv(vals[7], testRecv2, 4);
    printf("Received %s \n", testRecv2);
    free(testRecv);
    free(testRecv2);

    skipList_print();

    printf("-------- Deleting --------\n");
    for (unsigned long i  = 0; i < 12; i++) {
        if(skipList_del(vals[i]) == 0) {
            printf("Deleted ");
            printf("%lu", vals[i]);
            printf("\n");
        } else {
            printf("Could not delete ");
            printf("%lu", vals[i]);
            printf("\n");
        }
    }


    skipList_print();
    skipList_close();
    return 0;
}
