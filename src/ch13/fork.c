/*
 * Copyright (c) 2020, Dive into Systems, LLC
 * https://diveintosystems.org/
 *
 * Example of fork with calls to getpid to see differences in
 * parent and child process identifiers
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {

    int pid, mypid;

    printf("A\n");

    pid = fork();   /* create a new child process */
    if(pid == -1) {  /* check and handle error return value */
        printf("fork failed!\n");
        exit(pid);
    }
    if (pid == 0) { /* the child process */
        mypid = getpid();
        printf("Child: fork returned %d, my pid %d\n", pid, mypid);
    } else {  /* the parent process */
        mypid = getpid();
        printf("Parent: fork returned %d, my pid %d\n", pid, mypid);
    }
    printf("B:%d\n", mypid);

    return 0;
}

