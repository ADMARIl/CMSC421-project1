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
//#include <sys/types.h>
//#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <zconf.h>
//#include <rwlock.h>
//#include <cred.h>
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
    pid_t * accessList;
    int numUsers;
    // create a read write lock for a node on the skip list
    //rwlock_t slNodeLock;
};

unsigned int MAX_SL_SIZE = 0;
unsigned int PROB = 20000;
unsigned int SL_SIZE = 0;
bool INIT_STATE = false;
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
    if (ptrs < 1 || prob < 1)
        return EINVAL;
    // set the globals to their new values
    MAX_SL_SIZE = ptrs+1;
    PROB = prob;
    // seed random
    seed_random(time(NULL));
    // create the head and tail skip list nodes
    SL_TAIL = malloc(sizeof(struct skipList_node));
    SL_HEAD = malloc(sizeof(struct skipList_node));
    // set values to something we know is impossible
    SL_HEAD->id = 0;
    SL_TAIL->id = 0;
    // make them the max size so we can assign pointers correctly later
    SL_HEAD->towerHeight = MAX_SL_SIZE;
    SL_TAIL->towerHeight = MAX_SL_SIZE;
    // dynamically allocating nodes
    SL_HEAD->next = malloc(MAX_SL_SIZE * sizeof(struct skipList_node));
    SL_TAIL->next = malloc(sizeof(struct skipList_node));
    SL_TAIL->next[0] = NULL;

    // assign the next node of head to tail since there's nothing in the sl yet
    int i = 0;
    for (i = 0; i < ptrs; i++) {
        SL_HEAD->next[i] = SL_TAIL;
    }

    // set mailbox to null for now since we aren't using them yet
    SL_HEAD->mBox = NULL;
    SL_TAIL->mBox = NULL;
    // create empty access control list
    SL_HEAD->accessList = NULL;
    SL_TAIL->accessList = NULL;

    SL_HEAD->numUsers = 0;
    SL_TAIL->numUsers = 0;
    // tell the world that the skiplist is ready to roll
    INIT_STATE = true;
    return 0;
}

// search function
long skipList_search(unsigned long target) {
    // check if mailbox system has been initialized
    if (INIT_STATE == false)
        return ENODEV;
    // var so we know how high we are
    unsigned int currLevel = SL_SIZE;
    // node for us to traverse the list with
    struct skipList_node *currNode = SL_HEAD;
    // loop through all the nodes
    int i = 0;
    for (i = SL_SIZE; i >= 0; i--) {
        // check if we aren't at the bottom yet
        if (currLevel > 0) {
            currLevel--;
        }
        // loop to find anything to the right that isn't a tail
        while (currNode->next[currLevel]->id < target && currNode->next[currLevel] != SL_TAIL) {
            currNode = currNode->next[currLevel];
        }
    }
    // move forward one node so we get to the right spot
    currNode = currNode->next[currLevel];
    // see if we found it
    if (currNode->id == target)
        return 0;
    else
        return 1;
}

// insert function
static long skipList_add(unsigned long id) {
    // check if root
    uid_t uid = getuid();
    uid_t euid = geteuid();
    if (uid > 0 && euid > 0)
        return EPERM;
    // check if mailbox system has been initialized
    if (INIT_STATE == false)
        return ENODEV;
    // various vars to keep track of skipList parameters
    unsigned int currLevel = MAX_SL_SIZE - 1;
    struct skipList_node *currNode = SL_HEAD;
    struct skipList_node **nodes = malloc(MAX_SL_SIZE * sizeof(struct skipList_node *));
    // loop through levels to find target key
    int i = 0;
    for (i = MAX_SL_SIZE-1; i >= 0; i--) {
        // check if we aren't at the bottom yet
        if (currLevel > 0)
            currLevel--;
        // keep a history of everything as we go down
        nodes[i] = currNode;
        // loop to find anything to the right that isn't a tail
        while (currNode->next[currLevel]->id < id && currNode->next[currLevel] != SL_TAIL) {
            currNode = currNode->next[currLevel];
        }
    }

    // check if key already exists
    if (currNode->next[0]->id == id) {
        free(nodes);
        return EEXIST;
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
    newNode->id = id;
    newNode->towerHeight = newHeight;
    // set up empty mailbox
    newNode->mBox = malloc(sizeof(struct mailbox));
    newNode->mBox->numMessages = 0;
    // initialize linked list
    newNode->mBox->head = malloc(sizeof(struct mailBox_node));
    newNode->mBox->head->next = NULL;
    newNode->mBox->head->msg = NULL;

    newNode->accessList = NULL;
    newNode->numUsers = 0;

    newNode->next = malloc(newHeight * sizeof(struct skipList_node));

    // do pointer surgery to rebuild associations
    for (i = 0; i <= newHeight-1; i++) {
        newNode->next[i] = nodes[i]->next[i];
        nodes[i]->next[i] = newNode;
    }
    // increment size if we need to
    if (newHeight-1 > SL_SIZE) {
        SL_SIZE = newHeight-1;
    }

    free(nodes);

    return 0;
}

// delete function
long skipList_del(unsigned long id) {
    // check if root
    uid_t uid = getuid();
    uid_t euid = geteuid();
    if (uid > 0 && euid > 0)
        return EPERM;
    // check if mailbox system has been initialized
    if (INIT_STATE == false)
        return ENODEV;
    // check if exists first
    if (id < 0)
        return ENOENT;
    else {
        // various vars to keep track of skipList parameters
        unsigned int currLevel = SL_SIZE;
        unsigned int targetHeight = 0;
        struct skipList_node *currNode = SL_HEAD;
        struct skipList_node **nodes = malloc(SL_SIZE * sizeof(struct skipList_node *) * 2);
        // traverse through each level at a time
        int i = 0;
        for (i = SL_SIZE; i >= 0; i--) {
            // check if we aren't at the bottom yet
            if (currLevel > 0) {
                currLevel--;
                targetHeight++;
            }
            // keep a history of everything as we go down
            nodes[i] = currNode;
            // loop to find anything to the right that isn't a tail
            while (currNode->next[currLevel]->id < id && currNode->next[currLevel] != SL_TAIL) {
                currNode = currNode->next[currLevel];
            }
        }
        // node to keep track of data to help us re-stitch the list later
        currNode = currNode->next[currLevel];
        if (currNode->id == id) {
            // restitch changed pointers
            for (i = 0; i < currNode->towerHeight; i++)
                nodes[i]->next[i] = currNode->next[i];
            // free dynamically allocated stuff
            free(currNode->next);
            struct mailBox_node *currMboxNode = currNode->mBox->head;
            while (currMboxNode != NULL) {
                struct mailBox_node *tempNode = currMboxNode->next;
                free(currMboxNode->msg);
                free(currMboxNode);
                currMboxNode = tempNode;

            }
            free(currNode->accessList);
            free(currNode->mBox);
            free(currNode);
            free(nodes);
            return 0;
        }
        // return error if mailbox doesnt exist
        else {
            free(nodes);
            return ENOENT;
        }

    }

}

static int skipList_print() {
    // check if mailbox system has been initialized
    if (INIT_STATE == false)
        return ENODEV;
    printf("%s", "-------- Skip List -------- \n");
    // loop through all the levels of the list so we can print out everything
    int i = 0;
    for (i = 0; i < MAX_SL_SIZE-1; i++) {
        printf("Level ");
        printf("%d", i);
        printf("      ");
        // set current pointer to head node
        struct skipList_node *currNode = SL_HEAD;
        while (currNode->next[i]->id > 0) {
            // move to next node and print it
            currNode = currNode->next[i];
            printf("%lu", currNode->id);

            // print messages if we have them
            // make sure we aren't at the head or tail before we try to access the messages
            if (currNode != SL_HEAD && currNode != SL_TAIL) {
                printf(": ");
                // temp struct to help us traverse the messages
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
    return 0;
}

static long skipList_close() {
    // check if mailbox system has been initialized
    if (INIT_STATE == false)
        return ENODEV;
    // since the node "towers" are only pointers to themselves, we can traverse along the
    // bottom to delete them all, so set our starting node to the first node on level 0
    struct skipList_node *currNode = SL_HEAD->next[0];

    // loop through level 0 until we hit the tail
    while (SL_HEAD->next[0] != SL_TAIL) {
        // move us forward so we don't lose our pointers
        SL_HEAD->next[0] = currNode->next[0];
        // free all the dynamically allocated stuff in the node
        free(currNode->next);
        free(currNode->accessList);
        // free mailbox memory
        struct mailBox_node *currMboxNode = currNode->mBox->head;
        while (currMboxNode != NULL) {
            struct mailBox_node *tempNode = currMboxNode->next;
            free(currMboxNode->msg);
            free(currMboxNode);
            currMboxNode = tempNode;

        }
        // free remaining memory
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

    return 0;
}
//
// Mailbox Functions
//

long mBox_send(unsigned long id, const unsigned char *msg, long len) {
    // check if mailbox system has been initialized
    if (INIT_STATE == false)
        return ENODEV;

    // is length invalid?
    if (len < 0 || id == 0)
        return EINVAL;
    unsigned int currLevel = SL_SIZE;
    struct skipList_node *currNode = SL_HEAD;
    int i = 0;
    for (i = SL_SIZE; i >= 0; i--) {
        // check if we aren't at the bottom yet
        if (currLevel > 0) {
            currLevel--;
        }
        // loop to find anything to the right that isn't a tail
        while (currNode->next[currLevel]->id < id && currNode->next[currLevel] != SL_TAIL) {
            currNode = currNode->next[currLevel];
        }
    }
    currNode = currNode->next[currLevel];
    if (currNode->id != id)
        return ENOENT;

    // see if we are in the acl
    pid_t pid = getpid();
    bool access = false;
    if (currNode->accessList == NULL)
        access = true;
    else {
        for (i = 0; i < currNode->numUsers; i++) {
            if (currNode->accessList[i] == pid)
                access = true;
        }
    }

    // check if root
    uid_t uid = getuid();
    uid_t euid = geteuid();
    if ((uid > 0 && euid > 0) || access == false)
        return EPERM;

    struct mailBox_node *currMboxNode = currNode->mBox->head;
    for (i = 0; i < currNode->mBox->numMessages; i++) {
        currMboxNode = currMboxNode->next;
    }
    currMboxNode->next = malloc(sizeof(struct mailBox_node));
    currMboxNode->next->next = NULL;
    currMboxNode->next->msg = malloc(len * sizeof(char));
    currNode->mBox->numMessages++;
    memcpy(currMboxNode->next->msg, msg, len);
    return 0;
}

long mBox_recv(unsigned long id, unsigned char *msg, long len) {
    // check if mailbox system has been initialized
    if (INIT_STATE == false)
        return ENODEV;
    // are the values valid?
    if (len < 0 || id == 0)
        return EINVAL;
    unsigned int currLevel = SL_SIZE;
    struct skipList_node *currNode = SL_HEAD;
    int i = 0;
    for (i = SL_SIZE; i >= 0; i--) {
        // check if we aren't at the bottom yet
        if (currLevel > 0) {
            currLevel--;
        }
        // loop to find anything to the right that isn't a tail
        while (currNode->next[currLevel]->id < id && currNode->next[currLevel] != SL_TAIL) {
            currNode = currNode->next[currLevel];
        }
    }
    currNode = currNode->next[currLevel];
    if (currNode->id != id)
        return ENOENT;

    // see if we are in the acl or if the acl is null
    pid_t pid = getpid();
    bool access = false;
    if (currNode->accessList == NULL)
        access = true;
    else {
        for (i = 0; i < currNode->numUsers; i++) {
            if (currNode->accessList[i] == pid)
                access = true;
        }
    }

    // check if root
    uid_t uid = getuid();
    uid_t euid = geteuid();
    if ((uid > 0 && euid > 0) || access == false)
        return EPERM;

    struct mailBox_node *currMboxNode = currNode->mBox->head;
    if (currNode->mBox->numMessages == 0) {
        return ESRCH;
    }
    memcpy(msg, currMboxNode->next->msg, len);
    struct mailBox_node *tempNode = currNode->mBox->head->next->next;
    free(currNode->mBox->head->next->msg);
    free(currNode->mBox->head->next);
    free(currNode->accessList);
    currNode->mBox->head->next = tempNode;
    currNode->mBox->numMessages--;
    return 0;
}
// length
long mBox_nextSize(unsigned long id) {
    // check if mailbox system has been initialized
    if (INIT_STATE == false)
        return ENODEV;
    unsigned int currLevel = SL_SIZE;
    struct skipList_node *currNode = SL_HEAD;
    int i = 0;
    for (i = SL_SIZE; i >= 0; i--) {
        // check if we aren't at the bottom yet
        if (currLevel > 0) {
            currLevel--;
        }
        // loop to find anything to the right that isn't a tail
        while (currNode->next[currLevel]->id < id && currNode->next[currLevel] != SL_TAIL) {
            currNode = currNode->next[currLevel];
        }
    }
    currNode = currNode->next[currLevel];
    if (currNode->id != id)
        return ENOENT;
    if (currNode->mBox->head->next != NULL)
        return sizeof(currNode->mBox->head->next->msg);
    else
        // no messages?
        return ESRCH;

}
// count
long mBox_numMessages(unsigned long id) {
    // check if mailbox system has been initialized
    if (INIT_STATE == false)
        return ENODEV;
    unsigned int currLevel = SL_SIZE;
    struct skipList_node *currNode = SL_HEAD;
    int i = 0;
    for (i = SL_SIZE; i >= 0; i--) {
        // check if we aren't at the bottom yet
        if (currLevel > 0) {
            currLevel--;
        }
        // loop to find anything to the right that isn't a tail
        while (currNode->next[currLevel]->id < id && currNode->next[currLevel] != SL_TAIL) {
            currNode = currNode->next[currLevel];
        }
    }
    currNode = currNode->next[currLevel];
    if (currNode->id != id)
        return ENOENT;
    return currNode->mBox->numMessages;

}

//
// Access Control List Functions
//

long acl_print(unsigned long id) {
    // check if root
    uid_t uid = getuid();
    uid_t euid = geteuid();
    if (uid > 0 && euid > 0)
        return EPERM;
    // check if mailbox system has been initialized
    if (INIT_STATE == false)
        return ENODEV;
    unsigned int currLevel = SL_SIZE;
    struct skipList_node *currNode = SL_HEAD;
    int i = 0;
    for (i = SL_SIZE; i >= 0; i--) {
        // check if we aren't at the bottom yet
        if (currLevel > 0) {
            currLevel--;
        }
        // loop to find anything to the right that isn't a tail
        while (currNode->next[currLevel]->id < id && currNode->next[currLevel] != SL_TAIL) {
            currNode = currNode->next[currLevel];
        }
    }
    currNode = currNode->next[currLevel];
    if (currNode->id != id)
        return ENOENT;
    printf("Access Control List for id:%lu ", id);
    for (i = 0; i < currNode->numUsers; i++) {
        printf("[pid = %d]", currNode->accessList[i]);
    }
    printf("\n");
    return 0;
}

long acl_add(unsigned long id, pid_t process_id) {
    // check if root
    uid_t uid = getuid();
    uid_t euid = geteuid();
    if (uid > 0 && euid > 0)
        return EPERM;

    // check if mailbox system has been initialized
    if (INIT_STATE == false)
        return ENODEV;
    unsigned int currLevel = SL_SIZE;
    struct skipList_node *currNode = SL_HEAD;
    int i = 0;
    for (i = SL_SIZE; i >= 0; i--) {
        // check if we aren't at the bottom yet
        if (currLevel > 0) {
            currLevel--;
        }
        // loop to find anything to the right that isn't a tail
        while (currNode->next[currLevel]->id < id && currNode->next[currLevel] != SL_TAIL) {
            currNode = currNode->next[currLevel];
        }
    }
    currNode = currNode->next[currLevel];
    if (currNode->id != id)
        return ENOENT;
    currNode->numUsers++;
    if (currNode->accessList == NULL) {
        currNode->accessList = malloc(sizeof(pid_t) * 2);
    } else if ((sizeof(pid_t) * currNode->numUsers) < sizeof(currNode->accessList)) {
        pid_t * tempList;
        tempList = realloc(currNode->accessList, 2 * sizeof(pid_t) * currNode->numUsers);
        currNode->accessList = tempList;
    }
    for (i = 0; i < currNode->numUsers; i++) {
        if (currNode->accessList[i] == process_id) {
            currNode->numUsers--;
            // return error if the id already exists
            return EINVAL;
        }
    }
    currNode->accessList[currNode->numUsers - 1] = process_id;
    printf("Added pid of %d to mailbox %lu at position %d\n", process_id, id, i);
    acl_print(id);
    return 0;

}

long acl_remove(unsigned long id, pid_t process_id) {
    // check if root
    uid_t uid = getuid();
    uid_t euid = geteuid();
    if (uid > 0 && euid > 0)
        return EPERM;

    // check if mailbox system has been initialized
    if (INIT_STATE == false)
        return ENODEV;
    unsigned int currLevel = SL_SIZE;
    struct skipList_node *currNode = SL_HEAD;
    int i = 0;
    for (i = SL_SIZE; i >= 0; i--) {
        // check if we aren't at the bottom yet
        if (currLevel > 0) {
            currLevel--;
        }
        // loop to find anything to the right that isn't a tail
        while (currNode->next[currLevel]->id < id && currNode->next[currLevel] != SL_TAIL) {
            currNode = currNode->next[currLevel];
        }
    }
    currNode = currNode->next[currLevel];
    if (currNode->id != id)
        return ENOENT;

    for (i = 0; i < currNode->numUsers; i++) {
        if (currNode->accessList[i] == process_id) {
            int j = 0;
            for (j = i; j < currNode->numUsers; j++)
                currNode->accessList[i] = currNode->accessList[i+1];
            currNode->numUsers--;
            printf("Removed pid of %d at mailbox %lu at position %d\n", process_id, id, j);
        }
    }

    acl_print(id);
    return 0;
}

#endif //SKIPLIST_SKIPLIST_H
