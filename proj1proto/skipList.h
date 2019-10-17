/*
 *  File: skipList.h
 *  Author: Andrew Ingson (aings1@umbc.edu)
 *  Date; 10/13/19
 *  CMSC 421 (Principles of Operating Systems) Project 1
 *
 *  Yo dog I head you like probabilistic alternatives to lists so I wrote some
 *  code for skipList functions
 *
 *  TODO: Adjust code to work in kernel space
 *  TODO: Add more comments
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

// structs to hold our node data
typedef struct skipList_head {
    struct skipList_node *headNode;
    //int height, length;
    bool empty;
} head_t;

typedef struct skipList_node {
    //struct list_head link;
    struct skipList_node *right;
    int key;
} node_t;

unsigned int MAX_SIZE = 0;
unsigned int PROB = 20000;
head_t *LEVELS = NULL;

// provided random generation functions
static unsigned int next_random = 9001;

static unsigned int generate_random_int(void) {
    next_random = next_random * 1103515245 + 12345;
    return (next_random / 65536) % 32768;
}

static void seed_random(unsigned int seed) {
    next_random = seed;
}

// initialization function
long skipList_initialize(unsigned int ptrs, unsigned int prob) {
    //skipList->height = 1;
    //skipList->length = 0;
    MAX_SIZE = ptrs;
    PROB = prob;
    // not sure exactly what this does yet but I think this is what I nee
    /*struct list_head main_head;
    INIT_LIST_HEAD(&main_head);
    list_add(skipList, &main_head);

    //struct list_head heads[MAX_SIZE];*/

    for (int i = 0; i < MAX_SIZE; i++) {
        //LEVELS[i] = NULL;
        //INIT_LIST_HEAD(&LEVELS[i]);
        head_t* new_head = (head_t* ) malloc(sizeof(head_t));
        node_t* new_node = (node_t* ) malloc(sizeof(node_t));
        //struct skipList_node head;
        //head.link = LEVELS[i];
        //new_head->height = i;
        //new_head->length = 0;
        new_head->empty = true;
        new_head->headNode = new_node;
        new_node->right = NULL;
        //list_add_tail(&new_node->link, &LEVELS[i]);
        LEVELS[i] = *new_head;
    }

/*    // LEVELS = heads;
   // skipList->headNode->link = main_head;
    //skipList->headNode->right = NULL;*/
    return 0;
}

// insert function
static int skipList_add(int key) {
    //list_add(new,head);
    int currLevel = 0;
    bool success = false;
    struct skipList_node *curr = LEVELS[0].headNode;
    if (LEVELS[0].empty == true) {
        do {
            node_t *new_node = (node_t *) malloc(sizeof(node_t));
            new_node->key = key;
            new_node->right = NULL;
            curr->right = new_node;
        } while ((PROB - next_random) > 0);
        LEVELS[0].empty = false;
        //success = true;
    } else {
        while (!success) {
            generate_random_int();
            if (curr->right->key <= key) {
                curr = curr->right;
            } else if (curr->right->key > key && LEVELS[currLevel+1].empty == false) {
                currLevel++;
            } else if (curr->right == NULL) {
                do {
                    node_t *new_node = (node_t *) malloc(sizeof(node_t));
                    new_node->key = key;
                    new_node->right = NULL;
                    curr->right = new_node;
                } while ((PROB - next_random) > 0);
                LEVELS[0].empty = false;
                success = true;
            }
        }
    }
    return key;
}

// delete function
int skipList_del(int target) {
    // TODO: Delete function
    if (LEVELS[0].empty == true) {
        return -1;
    } else {
        for (int i = 0; i < MAX_SIZE; i++) {
            struct skipList_node *curr = LEVELS[i].headNode;
            struct skipList_node *currPrev = curr;
            while (curr->right != NULL) {
                if (curr->key == target) {
                    printf("Target found\n");
                    currPrev->right = curr->right;
                    //return target;
                } else {
                    currPrev = curr;
                    curr = curr->right;
                }
            }
        }
        return 0;
    }
}

// search function
int skipList_search(int target) {
    if (LEVELS[0].empty == true) {
        return -1;
    } else {
        for (unsigned int i = 0; i < MAX_SIZE; i++) {
            struct skipList_node *curr = LEVELS[i].headNode;
            while (curr->right != NULL) {
                if (curr->key == target) {
                    printf("Target found\n");
                    return target;
                } else {
                    curr = curr->right;
                }
            }
        }
        return 0;
    }
}

static void skipList_print() {
    printf("%s/n", "-------- Skip List --------");
    if (LEVELS[0].empty == true) {
        printf("List is empty.\n");
    } else {
        for (unsigned int i = 0; i < MAX_SIZE; i++) {
            struct skipList_node *curr = LEVELS[i].headNode;
            while (curr->right != NULL) {
                printf("%d", curr->key);
                printf(" \n");
            }
            printf("\n");
        }
    }
}

#endif //SKIPLIST_SKIPLIST_H
