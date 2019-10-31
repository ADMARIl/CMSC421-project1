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
#include <string.h>
#include "list.h"

// structs to hold our node data

struct mailBox_node {
    unsigned char* msg;
    struct mailBox_node* next;
};

struct mailbox {
    int numMessages;
    struct mailBox_node *head;
};

struct skipList_node {
    //struct list_head link;
    unsigned long id;
    unsigned int towerHeight;
    struct skipList_node** next;
    struct mailbox* mBox;
};

unsigned int MAX_SL_SIZE = 0;
unsigned int PROB = 20000;
unsigned int SL_SIZE = 0;
struct skipList_node *SL_HEAD;
struct skipList_node *SL_TAIL;

//
// provided random generation functions
//
static unsigned int next_random = 9001;

static unsigned int generate_random_int(void) {
    next_random = next_random * 1103515245 + 12345;
    return (next_random / 65536) % 32768;
}

static void seed_random(unsigned int seed) {
    next_random = seed;
}

//
// SkipList functions
//
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
    SL_HEAD->id = 0;
    SL_TAIL->id = 0;

    SL_HEAD->towerHeight = MAX_SL_SIZE;
    SL_TAIL->towerHeight = MAX_SL_SIZE;

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
static long skipList_add(unsigned long key) {
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
    unsigned int newHeight = 1;
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

    // assign the pointers ahead and behind
    struct skipList_node *newNode = malloc(sizeof(struct skipList_node));
    newNode->id = key;
    newNode->towerHeight = newHeight;
    // set up empty mailbox
    newNode->mBox = malloc(sizeof(struct mailbox));
    newNode->mBox->numMessages = 0;

    newNode->mBox->head = malloc(sizeof(struct mailBox_node));
    newNode->mBox->head->next = NULL;
    newNode->mBox->head->msg = NULL;

    newNode->next = malloc(newHeight * sizeof(struct skipList_node));

    for (int i = 0; i <= newHeight-1; i++) {
        newNode->next[i] = nodes[i]->next[i];
        nodes[i]->next[i] = newNode;
    }

    if (newHeight-1 > SL_SIZE) {
        SL_SIZE = newHeight-1;
    }

    free(nodes);

    return 0;
    //free(newNode);
}

// search function
long skipList_search(unsigned long target) {
    unsigned int currLevel = MAX_SL_SIZE - 1;
    //struct skipList_node *currNode = SL_HEAD;

    for (int i = 0; i < MAX_SL_SIZE; i++) {

        struct skipList_node *currNode = SL_HEAD;
        while (currNode->next[i]->id > 0) {
            if (currNode->next[i]->id == target)
                return 0;
            else
                currNode = currNode->next[i];
        }
    }

    return 1;
}

// delete function
long skipList_del(unsigned long target) {

    if (skipList_search(target) != 0 || target < 0)
        return 1;
    else {

        unsigned int currLevel = SL_SIZE;
        unsigned int targetHeight = 0;
        struct skipList_node *currNode = SL_HEAD;
        struct skipList_node **nodes = malloc(SL_SIZE * sizeof(struct skipList_node *));

        for (int i = SL_SIZE; i >= 0; i--) {
            // check if we aren't at the bottom yet
            if (currLevel > 0) {
                currLevel--;
                targetHeight++;
            }
            // keep a history of everything as we go down
            nodes[i] = currNode;
            // loop to find anything to the right that isn't a tail
            while (currNode->next[currLevel]->id < target && currNode->next[currLevel] != SL_TAIL) {
                currNode = currNode->next[currLevel];
            }
        }

        //struct skipList_node *freeNode = currNode;
        currNode = currNode->next[currLevel];
        if (currNode->id == target) {
            for (int i = 0; i < currNode->towerHeight; i++)
                nodes[i]->next[i] = currNode->next[i];

            free(nodes);
            free(currNode->next);
            /*struct mailBox_node *currMboxNode = currNode->mBox->head;
            while (currMboxNode->next != NULL) {
                free(currMboxNode);
                currMboxNode = currMboxNode->next;
            }*/
            //free(currNode->mBox->head);
            free(currNode->mBox);
            free(currNode);
            return 0;
        }
        // free(currNode);

        else {
            return 1;
        }

    }

}

static void skipList_print() {
    printf("%s", "-------- Skip List -------- \n");

    for (int i = 0; i < MAX_SL_SIZE; i++) {
        printf("Level ");
        printf("%d", i);
        printf("      ");

        struct skipList_node *currNode = SL_HEAD;
        while (currNode->next[i]->id > 0) {
            currNode = currNode->next[i];
            printf("%lu", currNode->id);

            // make sure we aren't at the head or tail before we try to access the messages
            if (currNode != SL_HEAD && currNode != SL_TAIL) {
                printf(": ");
                struct mailBox_node *currMboxNode = currNode->mBox->head;
                while (currMboxNode->next != NULL) {
                    printf("[message: %s] ", currMboxNode->next->msg);
                    currMboxNode = currMboxNode->next;
                }
            } else {
                printf(" ");
            }

        }
        printf("\n");
    }
}

static void skipList_close() {
    // since the node "towers" are only pointers to themselves, we can traverse along the
    // bottom to delete them all, so set our starting node to the first node on level 0
    struct skipList_node *currNode = SL_HEAD->next[0];

    // loop through level 0 until we hit the tail
    while (SL_HEAD->next[0] != SL_TAIL) {
        // move us forward so we don't lose our pointers
        SL_HEAD->next[0] = currNode->next[0];
        // free all the dynamically allocated stuff in the node
        free(currNode->next);

        struct mailBox_node *currMboxNode = currNode->mBox->head;
        while (currMboxNode->next != NULL) {
            free(currMboxNode);
            currMboxNode = currMboxNode->next;
        }
        //free(currNode->mBox->head);
        free(currNode->mBox);
        free(currNode);

        // check to see if we are at the end yet
        if (SL_HEAD->next[0] != SL_TAIL) {
            currNode = currNode->next[0];
        }
    }
    // free our beginning nodes
    free(SL_TAIL->next);
    free(SL_TAIL);
    free(SL_HEAD->next);
    free(SL_HEAD);

}
//
// Mailbox Functions
//

long mBox_send(unsigned long id, const unsigned char *msg, long len) {
    if (skipList_search(id) == 0){
        unsigned int currLevel = SL_SIZE;
        struct skipList_node *currNode = SL_HEAD;
        //struct skipList_node **nodes = malloc(SL_SIZE * sizeof(struct skipList_node *));

        for (int i = SL_SIZE; i >= 0; i--) {
            // check if we aren't at the bottom yet
            if (currLevel > 0) {
                currLevel--;
            }
            // keep a history of everything as we go down
            //nodes[i] = currNode;
            // loop to find anything to the right that isn't a tail
            while (currNode->next[currLevel]->id < id && currNode->next[currLevel] != SL_TAIL) {
                currNode = currNode->next[currLevel];
            }
        }

        currNode = currNode->next[currLevel];
        struct mailBox_node *currMboxNode = currNode->mBox->head;
        for (int i = 0; i < currNode->mBox->numMessages; i++) {
            currMboxNode = currMboxNode->next;
        }

        currMboxNode->next = malloc(sizeof(struct mailBox_node));
        currMboxNode->next->msg = malloc(sizeof(msg));
        currNode->mBox->numMessages++;

        memcpy(currMboxNode->next->msg, msg, len);

    }
    return 0;
}

long mBox_recv(unsigned long id, unsigned char *msg, long len) {
    if (skipList_search(id) == 0){
        unsigned int currLevel = SL_SIZE;
        struct skipList_node *currNode = SL_HEAD;
        //struct skipList_node **nodes = malloc(SL_SIZE * sizeof(struct skipList_node *));

        for (int i = SL_SIZE; i >= 0; i--) {
            // check if we aren't at the bottom yet
            if (currLevel > 0) {
                currLevel--;
            }
            // keep a history of everything as we go down
            //nodes[i] = currNode;
            // loop to find anything to the right that isn't a tail
            while (currNode->next[currLevel]->id < id && currNode->next[currLevel] != SL_TAIL) {
                currNode = currNode->next[currLevel];
            }
        }

        struct mailBox_node *currMboxNode = currNode->mBox->head;
        /*for (int i = 0; i <= currNode->mBox->numMessages; i++) {
            currMboxNode = currMboxNode->next;
        }*/

        if (currMboxNode->next != NULL) {
            memcpy(msg, currMboxNode->next->msg, len);
            //free(currMboxNode->next);
            currNode->mBox->head->next = currMboxNode->next->next;
        } else {
            // TODO: return error
            return 1;
        }
    }
    return 0;
}

long mBox_length(unsigned long id) {
    if (skipList_search(id) == 0){
        unsigned int currLevel = SL_SIZE;
        struct skipList_node *currNode = SL_HEAD;
        //struct skipList_node **nodes = malloc(SL_SIZE * sizeof(struct skipList_node *));

        for (int i = SL_SIZE; i >= 0; i--) {
            // check if we aren't at the bottom yet
            if (currLevel > 0) {
                currLevel--;
            }
            // keep a history of everything as we go down
            //nodes[i] = currNode;
            // loop to find anything to the right that isn't a tail
            while (currNode->next[currLevel]->id < id && currNode->next[currLevel] != SL_TAIL) {
                currNode = currNode->next[currLevel];
            }
        }

        return currNode->mBox->numMessages;
    }

}

//
// Access Control List Functions
//

long acl_add(unsigned long id, pid_t process_id) {

}

long acl_remove(unsigned long id, pid_t process_id) {

}
#endif //SKIPLIST_SKIPLIST_H
