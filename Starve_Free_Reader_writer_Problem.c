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
