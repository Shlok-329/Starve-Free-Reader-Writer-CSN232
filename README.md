<h2> Starve-Free Readers Writers Problem </h2>

***SEMAPHORES***
1. **resource** : It is the Semaphore which is used to prevent readers and writers from executing critical section code simultaneously.
2. **r_mutex**  : It is the Semaphore which is used to provide mutual exclusion to readers_count variable which is currently executing the code in critical section.
3. **rw_queue** : It is the Semaphore which maintains the order of arrival of readers and writers.

***Global (or Shared) variables***
1. **data** : It is a part of program which is shared among various threads.
2. **readers_count** : It is an int type variable which stores the count of number of readers present in critical section at any point of time.

***Reader Function***
- **Entry Section** :
  1. First of reader enters the request queue and waits till it gets the **rw_queue** semaphore.
  2. Now reader tries to aquire **r_mutex** semaphore which allow reader to modify readers_count.
  3. Now, reader waits till it gets the **resource** semaphore if it is the only reader which is currently under execution.
  4. Now, **rw_queue** and **r_mutex** semaphores are released before entering into the critical section.

- **Critical Section** : In this section, reader reads the global data variable.

- **Exit Section** : 
  1. First of all, Reader waits till it gets the **r_mutex** semaphore to modify readers_count variable.
  2. Now, if it is the last reader, it releases the **resource** semaphore.
  3. At last, **r_mutex** semaphore is released after modifying readers_count variable.

***Writer Function***
- **Entry Section** :
  1. First of all, writer enters into the request queue and waits till it gets the **rw_queue** semaphore.
  2. Now, it waits till it gets **resource** semaphore and enter in the critical section.
  3. At last, **rw_queue** semaphore is released before entering into the critical section.

- **Critical Section** : In this section, writer modifies the shared data among various threads.

- **Exit Section** :
  1. In this section, writer releases the **resource** semaphore.


***Main Function***
1. Firstly, Count of number of readers and writers are taken as input from the user.
2. Now, Threads are created corresponding to the number of readers and writers value we get from the user.
3. Semaphores are now initialized.
4. Now, we have to initialize readers and writers threads with corresponding functions.
5. Now, all threads are joined back to the parent thread.
6. At last, all semaphores are destroyed after finishing their execution.
