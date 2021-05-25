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

/* CPP code for the Starve Free Reader Writer Problem */

#include <iostream>
using namespace std;

// A queue node.
struct Node
{
    // Pointer to next node and process_id for current process
    Node* next;
    pid_t value;
};


class Queue
{
    // Front pointer for pop operation
    // Rear pointer for adding a new node
    Node* Front, *Rear;
    
    public:
    // Method to push a node at end of queue
    void push(pid_t val)
    {
        // Create a node and initialize pid value
        Node* new_process = new Node();
        new_process->value = val;
        
        // Update front if queue is empty
        if(Rear != NULL)
            Rear->next = new_process;
        else
            Front = new_process;
        
        // Update Rear to point to added node
        Rear = new_process;
    }
    
    // Method to pop a Node from front of queue
    pid_t pop()
    { 
        // check if queue if empty
        if(Front == NULL)
        {
            // Queue is Empty : Underflow Error
            return -1;
        }
        else
        {
            // Get Front node
            pid_t process_id = Front->value;
            
            // Update Front node
            Front = Front->next;
            
            // Update Rear if queue is empty now
            if(Front == NULL)
                Rear = NULL;
            
            // Return the value of procss id
            return process_id;
        }
    }
};

// C++-Style Struct Implementation of Semaphore
class Semaphore
{
    
    int value = 1;
    // Queue to Store Processes waiting to be executed
    Queue processes = Queue();
    
    public:
    // wait method to block other process while 
    // a code executes inside critical section
    void wait(pid_t process_id)
    {
        value--;
        if(value < 0)
        {
            processes.push(process_id);
            
            // Block other processes using non-busy waiting
            // until signal() is called after execution of 
            // Critical Section by executing process.
            wait(process_id); 
        }
    }
    
    void signal()
    {
        value++;
        if(value <= 0)
        {
            pid_t pid = processes.pop();
            // Wakeup the next process for execution from queue
            wakeup(pid); 
        }
    }
};


//Declare Semaphores
struct Semaphore rw_queue;
struct Semaphore resource;
struct Semaphore r_mutex;

// Initialize readers count
int reader_count = 0;

// Initial critical data section to be shared
int data = 1;

void *writer(pid_t process_id){

    /*-------------- ENTRY section ------------------*/

    // Wait in queue till other requests are serviced
    rw_queue.wait(process_id);
    // Wait its turn to modify resource semaphore
    resource.wait(process_id);
    // Release the rw_queue semaphore lock for other process
    rw_queue.signal();
    


    /*------------- CRITICAL section ----------------*/
    
    // Perform any modification of shared data variable here
    data = data*2;


    /*-------------- EXIT section -------------------*/

    // Release the resource semaphore lock for next process
    resource.signal();
}

void *reader(pid_t process_id){
    
    /*-------------- ENTRY section ------------------*/

    // Wait in queue till other requests are serviced
    rw_queue.wait(process_id);
    // Only one process should update readers count at a time
    r_mutex.wait(process_id);
    // Incrememt readers count
    reader_count++;

    // If current reader is the first reader wait till
    // other writers release the resource semaphore
    if(reader_count == 1)
        resource.wait(process_id);

    // Allow other process in queue to be serviced
    rw_queue.signal();
    // Readers count is updated
    r_mutex.signal();
    
    

    /*------------- CRITICAL section ----------------*/
    
    // Perform any read operation of data variable from database
    printf("%d",data);
    


    /*-------------- EXIT section -------------------*/

    // Only one process should update readers count at a time
    r_mutex.wait(process_id);
    // Decrement read count semaphore after reading is done
    reader_count--;

    // Release resource semaphore for writer if
    // no other reader is remaining or left.
    if(reader_count == 0) 
        resource.signal();
        
    // Allow other readers to modify readcount semaphore
    r_mutex.signal();

}