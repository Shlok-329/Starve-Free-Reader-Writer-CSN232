/*  PseudoCode for Starve Free Reader Writer Problem

This problem is also called as Third Readers-Writers problem.
It requires that no thread shall be allowed to starve i.e., the operation of obtaining a lock on the shared data will always terminate in a bounded amount of time.
Here I present two solutions for the problem. One is the classical solution and the other is faster solution.
The faster solution is  fast as in it requires less number of semaphore lockings.

Classical Solution

Here, We are using
1. Semaphore "in" initialized to 1 to check which process is going to enter critical section.
2. Semaphore "mutex" initialized to 1 for mutual exculsion in reading processes while updating readers_count.
3. Semaphore "rw_mutex" initialized to 1 for mutual exclusion between writing processes.
4. Integer "readers_count" initialized to 0 to count number of reading processes.

//Structure of writer process

do{
  wait(in);

  wait(rw_mutex);

  ...
  // Writing is done //
  ...
  signal(rw_mutex);

  signal(in);

}while(true);

//Structure of reader process

do{
  wait(in);

  wait(mutex);

  readers_count++;

  if(readers_count==1){
    wait(rw_mutex);
  }

  signal(mutex);

  signal(in);

  ...
  // reading is done //
  ...

  wait(mutex);

  reader_count--;

  if(readers_count==0){
    signal(rw_mutex);
  }

  signal(mutex);

}while(true);


1. Writer Process: Lock "in" semaphore to prevent reader process from reading.
2. Lock rw_mutex to prevent other writing processes from writing. Do the writing then release the semaphores.
3. Readers can block writers by locking rw_mutex if readers obtained permission to read first.
4. Reader Process: Wait if there's any writer using "in". Lock "mutex" to update readers_count and if its the first reader lock "rw_mutex" to
prevent writers from writing. Release "in" and "mutex". Execute the reading process.
5. Again lock "mutex" to update readers_count and if its last reading process release "rw_mutex" to allow writers to write.

Faster Solution

Faster Solution (Reader) - 1 mutex for entering and 1 mutex for Leaving.
Classical Solution (Reader) - 2 mutex for entering and 1 mutex for leaving.

Here, we are using
1. Semaphore "in" initialized to 1 for a process entering.
2. Semaphore "out" initialized to 1 for a proccess exiting.
3. Semaphore "rw_mutex" initialized to 0 for exclusion of writing and reading processes.
4. Integer "in_count" initialized to 0 number of entered reading processes.
5. Integer "out_count" initialized to 0 number of exited reading processes.
6. Boolean "wait" initialized to false to check if a writer is waiting.

//Structure of Writer process

do{

  wait(in);

  wait(out);

  if(in_count==out_count){
    signal(out);
  }else{
    wait=true;

    signal(out);

    wait(rw_mutex);

    wait=false;
  }

  ...
  // Writing is done //
  ...

  signal(in);

}while(true);

//Structure of Reader proces

do{
  wait(in);

  in_count++;

  signal(in);

  ...
  // Reading is done //
  ...

  wait(out);

  out_count++;

  if(wait && in_count==out_count){
    signal(rw_mutex);
  }

  signal(out);

}while(true);

1. Writer process : Lock "in" to prevent any new reader from entering. If number of entered reading process equal to exited reading process then just continue with writing process
		    else mark "wait" equal to true to let readers know that writer processes are waiting. Lock "rw_mutex" when readers allow it to prevent other readers and writers
		    from accessing the resource. When "rw_mutex" is acquired "wait" is marked false and writing is done. Release **in** in the end.

2. Reader process : Lock "in" to update in_count. If writer has not locked it already then it is now occupied by the reader. On updating release "in" for other processes. Reading
		    is done. Lock "out" to update out_count. If some writer is waiting and all entered readers have exited then "rw_mutex" is released to allow writers access to
		    the resource.


*/

/* C code for the Starve Free Reader Writer Problem */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

int data = 1;

int readers_count = 0;

sem_t rw_queue,resource,r_mutex;

void *reader(void *rid){
    /*-------------- ENTRY section ------------------*/

    sem_wait(&rw_queue);

    sem_wait(&r_mutex);

    readers_count++;

    if(readers_count==1){
        sem_wait(&resource);
    }

    sem_post(&rw_queue);

    sem_post(&r_mutex);

    /*------------- CRITICAL section ----------------*/

    printf("Reader %d have read data as %d\n",*((int *)rid),data);

    /*-------------- EXIT section -------------------*/

    sem_wait(&r_mutex);

    readers_count--;

    if(!readers_count){
        sem_post(&resource);
    }

    sem_post(&r_mutex);
}

void *writer(void *wid){
    /*-------------- ENTRY section ------------------*/

    sem_wait(&rw_queue);

    sem_wait(&resource);

    sem_post(&rw_queue);

    /*------------- CRITICAL section ----------------*/

    data *= 2;
    printf("Writer %d have modified data to %d\n",(*((int *)wid)),data);

    /*-------------- EXIT section -------------------*/

    sem_post(&resource);
}

int main(){
    int READERS,WRITERS,MAX_ID,i;

    printf("Enter the number of Readers: ");
    scanf("%d", &READERS);

    printf("Enter the number of Writers: ");
    scanf("%d", &WRITERS);

    MAX_ID = READERS >= WRITERS ? READERS : WRITERS;

    int id_arr[MAX_ID];
    for(i=0 ; i<MAX_ID ; i++){
        id_arr[i] = i+1;
    }

    pthread_t read[READERS],write[WRITERS];

    sem_init(&rw_queue,0,1);

    sem_init(&r_mutex,0,1);

    sem_init(&resource,0,1);

    for(i=0 ; i<READERS ; i++){
        pthread_create(&read[i], NULL, (void *)reader, (void *)&id_arr[i]);
    }

    for(i=0 ; i<WRITERS ; i++){
        pthread_create(&write[i], NULL, (void *)writer, (void *)&id_arr[i]);
    }

    for(i=0 ; i<READERS ; i++){
        pthread_join(read[i], NULL);
    }

    for(i=0 ; i<WRITERS ; i++){
        pthread_join(write[i], NULL);
    }

    sem_destroy(&rw_queue);

    sem_destroy(&r_mutex);

    sem_destroy(&resource);

    return 0;
}
