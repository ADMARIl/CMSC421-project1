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
    //seed_random(time(NULL));
    seed_random(1000000);
    // test vals
    unsigned long vals[] = {5, 3, 2, 8, 14, 443, 80, 21, 22, 2, 56, 66};

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

    const unsigned char testMsg[4] = {'t','e','s','t'};
    mBox_send(vals[4], (const unsigned char *) testMsg, 4);

    skipList_print();

    printf("-------- Search for each value --------\n");
    vals[7] = 188;
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
