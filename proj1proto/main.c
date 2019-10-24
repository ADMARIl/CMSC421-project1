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

unsigned int SKIPLIST_SIZE = 10;

int main() {
    // generate probability
    //seed_random(time(NULL));
    // test vals
    int vals[] = {5, 3, 2, 8, 14, 443, 80, 21, 22, 1, 56, 66};

    // start skipList
    // struct skipList_head skipList;
    skipList_initialize(SKIPLIST_SIZE, 16383);

    // add to skipList
    printf("-------- Inserting --------\n");
    for (unsigned long i  = 0; i < 12; i++) {
        //printf("Adding ");
        skipList_add(vals[i]);
        //printf("%ld", );

    }

    skipList_print();

   /* printf("-------- Search for each value --------\n");
    for (unsigned long i  = 0; i < sizeof(vals); i++) {
        if(skipList_search(vals[i] != -1)) {
            printf("Found ");
            printf("%d", vals[i]);
        }
    }

    printf("-------- Deleting --------\n");
    for (unsigned long i  = 0; i < sizeof(vals); i++) {
        if(skipList_del(vals[i] != -1)) {
            printf("Deleted ");
            printf("%d", vals[i]);
        }
    }*/
    return 0;
}
