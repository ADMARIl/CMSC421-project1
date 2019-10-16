/*
 *  File: skipList.h
 *  Author: Andrew Ingson (aings1@umbc.edu)
 *  Date; 10/13/19
 *  CMSC 421 (Principles of Operating Systems) Project 1
 *
 *  Yo dog I head you like probabilistic alternatives to lists so I wrote some
 *  code for skipList functions
 *
 _____ _     _       _
|_   _| |   (_)     (_)
  | | | |__  _ ___   _ ___    __ _
  | | | '_ \| / __| | / __|  / _` |
  | | | | | | \__ \ | \__ \ | (_| |
__\_/_|_| |_|_|___/_|_|___/  \__,_|____ _                 _
| ___ \   (_)     | |            |  ___(_)               | |
| |_/ /__  _ _ __ | |_ ___ _ __  | |_   _  __ _ ___  ___ | |
|  __/ _ \| | '_ \| __/ _ \ '__| |  _| | |/ _` / __|/ _ \| |
| | | (_) | | | | | ||  __/ |    | |   | | (_| \__ \ (_) |_|
\_|  \___/|_|_| |_|\__\___|_|    \_|   |_|\__,_|___/\___/(_)

 */
#ifndef SKIPLIST_SKIPLIST_H
#define SKIPLIST_SKIPLIST_H

// include all the things

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "list.h"

unsigned int MAX_SIZE = 0;
unsigned int PROB = 20000;
struct list_head *LEVELS = NULL;

// structs to hold our node data
struct skipList_head {
    struct skipList_node *headNode;
    int height, length;
};

struct skipList_node {
    struct list_head link;
    struct skipList_node *bellow;
    int key, prob;
};

// initialization function
long skipList_initialize(unsigned int ptrs, unsigned int prob) {
    //skipList->height = 1;
    //skipList->length = 0;
    MAX_SIZE = ptrs;
    PROB = prob;
    // not sure exactly what this does yet but I think this is what I nee
    //struct list_head main_head;
    //NIT_LIST_HEAD(&main_head);
    //list_add(skipList, &main_head);

    struct list_head heads[MAX_SIZE];

    for (int i = 0; i < MAX_SIZE; i++) {
        //heads[i] = NULL;
        INIT_LIST_HEAD(&heads[i]);
        //struct skipList_node head;
        //head.link = heads[i];
        //head
    }

    LEVELS = heads;
   // skipList->headNode->link = main_head;
    //skipList->headNode->right = NULL;
    return 0;
}

// insert function
static int skipList_add(int key) {
    //list_add(new,head);
    return key;
}

// delete function
static void skipList_del(struct list_head *entry) {
    list_del(entry);
}

// search function
static void skipList_search(struct list_head *target, struct list_head *head) {
    // stuff needs to go here
}

static void skipList_print(struct list_head *head) {
    printf("%s/n", "-------- Skip List --------");
}

// provided random generation functions
static unsigned int next_random = 9001;

static unsigned int generate_random_int(void) {
    next_random = next_random * 1103515245 + 12345;
    return (next_random / 65536) % 32768;
}

static void seed_random(unsigned int seed) {
    next_random = seed;
}

#endif //SKIPLIST_SKIPLIST_H
