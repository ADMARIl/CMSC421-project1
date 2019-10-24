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
/*typedef struct skipList_head {
    struct skipList_node *headNode;
    //int height, length;
    bool empty;
} head_t;*/

struct mailbox {
    int num;
};

struct skipList_node {
    //struct list_head link;
    int id;
    struct skipList_node** next;
    struct mailbox* mBox;
};

unsigned int MAX_SL_SIZE = 0;
unsigned int PROB = 20000;
struct skipList_node *SL_HEAD;
struct skipList_node *SL_TAIL;

//head_t *LEVELS = NULL;

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
    MAX_SL_SIZE = ptrs;
    PROB = prob;


    //struct skipList_node head;
    //SL_HEAD = head;

    SL_TAIL = malloc(sizeof(struct skipList_node));
    SL_HEAD = malloc(sizeof(struct skipList_node));
    // set values to something we know is impossible
    SL_HEAD->id = -1;
    SL_TAIL->id = -1;

    SL_HEAD->next = malloc(MAX_SL_SIZE * sizeof(struct skipList_node));

    SL_TAIL->next = malloc(sizeof(struct skipList_node));
    SL_TAIL->next[0] = NULL;

    // assign the next node of head to tail since there's nothing in the sl yet
    for (int i = 0; i < ptrs; i++) {
        SL_HEAD->next[i] = SL_TAIL;
    }

    // set mailbox to null for now since we aren't using them yet
    SL_HEAD->mBox = NULL;
    SL_TAIL->mBox = NULL;

    return 0;
}

// insert function
static long skipList_add(int key) {
    // various vars to keep track of skipList parameters
    unsigned int currLevel = MAX_SL_SIZE - 1;
    struct skipList_node *currNode = SL_HEAD;
    struct skipList_node **nodes = malloc(MAX_SL_SIZE * sizeof(struct skipList_node *));

    for (int i = MAX_SL_SIZE-1; i >= 0; i--) {
        // check if we aren't at the bottom yet
        if (currLevel > 0)
            currLevel--;
        // keep a history of everything as we go down
        nodes[i] = currNode;
        // loop to find anything to the right that isn't a tail
        while (currNode->next[currLevel]->id < key && currNode->next[currLevel] != SL_TAIL) {
            currNode = currNode->next[currLevel];
        }
    }

    // figure out how high we need to go
    unsigned int newHeight = currLevel;
    bool ceil = false;

    // figure out how high we need to go
    while (newHeight < MAX_SL_SIZE && !ceil) {
        // make sure we are very random
        // seed_random(time(NULL));

        unsigned int ranNum = generate_random_int();
        if ((32767 - ranNum) > PROB) {
            newHeight++;
            ranNum = generate_random_int();
        } else {
            ceil = true;
        }
    }

    printf("Adding key of ");
    printf("%d", key);
    printf(" with ");
    printf("%u ", newHeight);
    printf(" levels\n");

    // assign the pointers ahead and behind
    struct skipList_node *newNode = malloc(sizeof(struct skipList_node));
    newNode->id = key;
    newNode->mBox = malloc(sizeof(struct mailbox));
    newNode->mBox->num = key;

    printf("New height is ");
    printf("%d\n", newHeight);

    newNode->next = malloc(newHeight * sizeof(struct skipList_node));

    for (int i = 0; i <= newHeight; i++) {
        newNode->next[i] = nodes[i]->next[i];
        nodes[i]->next[i] = newNode;
    }
    /*//list_add(new,head);
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
    return key;*/
}

// search function
long skipList_search(int target) {
    unsigned int currLevel = MAX_SL_SIZE - 1;
    struct skipList_node *currNode = SL_HEAD;

    for (int i = 0; i < MAX_SL_SIZE; i++) {

        struct skipList_node *currNode = SL_HEAD;
        while (currNode->next[i]->id >= 0) {
            if (currNode->next[i]->id == target)
                return 0;
            else
                currNode = currNode->next[i];
        }
    }

    return 1;
}

// delete function
long skipList_del(int target) {
    // TODO: Delete function

    if (skipList_search(target) != 0)
        return 1;
    else {
        unsigned int currLevel = MAX_SL_SIZE - 1;
        /*struct skipList_node *currNode = SL_HEAD;
        struct skipList_node *aheadNode = malloc(sizeof(struct skipList_node));
        struct skipList_node **nodes = malloc(MAX_SL_SIZE * sizeof(struct skipList_node *));

        for (int i = MAX_SL_SIZE-1; i >= 0; i--) {
            // check if we aren't at the bottom yet
            if (currLevel > 0)
                currLevel--;
            // keep a history of everything as we go down
            nodes[i] = currNode;
            // loop to find anything to the right that isn't a tail
            while (currNode->next[currLevel]->id < target && currNode->next[currLevel] != SL_TAIL) {
                currNode = currNode->next[currLevel];
            }
        }*/
        struct skipList_node *currNode = SL_HEAD;

        for (int i = 0; i < MAX_SL_SIZE; i++) {

            while (currNode->next[i]->id >= 0) {
                if (currNode->next[i]->id == target) {
                    for (int i = 0; i < currLevel; i++) {
                        currNode->next[i] = currNode->next[currLevel]->next[currLevel];
                    }
                    return 0;
                }
                else
                    currNode = currNode->next[i];
            }
        }
        // aheadNode = currNode->next[currLevel]->next[currLevel];

        return 0;
    }
    return target;
}

static void skipList_print() {
    printf("%s", "-------- Skip List -------- \n");

    for (int i = 0; i < MAX_SL_SIZE; i++) {
        printf("Level ");
        printf("%d", i);
        printf("      ");

        struct skipList_node *currNode = SL_HEAD;
        while (currNode->next[i]->id > 0) {
            printf("%d", currNode->next[i]->id);
            printf(" ");
            currNode = currNode->next[i];
        }
        printf("\n");
    }
}

#endif //SKIPLIST_SKIPLIST_H
