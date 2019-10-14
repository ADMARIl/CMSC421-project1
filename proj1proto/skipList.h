/*
 *  File: skipList.h
 *  Author: Andrew Ingson (aings1@umbc.edu)
 *  Date; 10/13/19
 *  CMSC 421 (Principles of Operating Systems) Project 1
 *
 *  Code for skipList functions
 */
#ifndef SKIPLIST_SKIPLIST_H
#define SKIPLIST_SKIPLIST_H

// include all the things

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "list.h"

//struct list_head head_of_list;

// initialization function
static void skipList_intialize(struct list_head *first_list) {
    // not sure exactly what this does yet but I think this is what I need
    LIST_HEAD(test);
}

// insert function
static void skipList_add(struct list_head *new, struct list_head *head) {
    list_add(new,head);
}

// delete function
static void skipList_del(struct list_head *entry) {
    list_del(entry);
}

// search function
static void skipList_serach(struct list_head *target, struct list_head *head) {
    // stuff needs to go here
}


#endif //SKIPLIST_SKIPLIST_H
