#include "tests/lib.h"

#define NUM_THREADS 10

enum thread_action { THREAD_START, THREAD_STARTSOON, THREAD_STARTED };

int counter = 0;
int thread_status[NUM_THREADS] = { THREAD_START };

void countup(int me)
{
    int me2 = me;
    int i, j;
    for (j = 0; j <= me; j++) {
        for (i=0; i < (me+1)*10000; i++);
        printf("%d,%d: %d\n", me, me2, counter++);
    }
    printf("%d,%d: STOPPING\n", me,me2);
    thread_status[me] = 1;
    syscall_exit(0);
}


int main()
{
    int i;
    while (1) {
        for (i = 0; i < NUM_THREADS; i++) {
            switch (thread_status[i]) {
            case THREAD_START:
                thread_status[i] = THREAD_STARTED;
                syscall_fork(countup, i);
                break;
            case THREAD_STARTSOON:
                thread_status[i] = THREAD_START;
                break;
            }
        }
    }
    return counter;
}
