/*
 * This progam sends a virtual baton round in a ring of threads.
 */

#include "tests/lib.h"

// Number of threads in the ring
#define THREADS 5

// Number of times to make the baton go round
#define ROUNDS 3

usr_lock_t baton_lock;

typedef struct thread_data {
    int id;
    int baton;
    int countdown;
    struct thread_data *next;
    usr_cond_t cond;
} thread_data;


void thread_function(void *arg){
    thread_data *data = (thread_data *)arg;
    while(1) {

        syscall_lock_acquire(&baton_lock);
        
        // Wait until baton is non-zero (meaning that the thread
        // has the baton)
        while(data->baton == 0) {
            syscall_condition_wait(&data->cond, &baton_lock);
        }

        // First thread should perform countdown
        if(data->id == 0) {
            printf("\n%d: New round\n", data->id);
            if(--data->countdown == 0) {
                data->baton = -1; // create a stop baton
            }
        }

        // Pass the baton on to the next thread
        printf("%d: passing the baton on to %d\n", data->id,
                data->next->id);
        data->next->baton = data->baton;

        // Quit if the baton was a stop baton
        if(data->baton < 0){
            printf("%d: I quit.\n",data->id);
            syscall_lock_release(&baton_lock);
            syscall_condition_signal(&data->next->cond,
                    &baton_lock);
            syscall_exit(0);
        }

        //Remove baton from self and signal the next thread
        data->baton = 0;
        syscall_lock_release(&baton_lock);
        syscall_condition_signal(&data->next->cond, &baton_lock);
    }
}


int main() {
    syscall_lock_create(&baton_lock);

    // Setup data for threads
    thread_data data[THREADS];
    int i;
    for(i=0; i<(THREADS); i++) {
        data[i].id = i;
        data[i].baton = 0;
        data[i].countdown = 0;
        syscall_condition_create(&data[i].cond);
        data[i].next = &data[(i+1)%THREADS];
        syscall_fork((void (*)(int))(&thread_function), (int)&data[i]);
    }

    // Setup special data for the first thread
    data[0].countdown = ROUNDS;
    data[0].baton = 1;

    // Start first thread
    syscall_condition_signal(&data[0].cond, &baton_lock);

    syscall_exit(0);
    return 0;
}

