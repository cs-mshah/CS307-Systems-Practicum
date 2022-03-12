
### Problem 2 - Dining Students
- For 5 students and 5 spoons case, each person has a left and right spoon and when simulating, we had to make sure that only person could use a given spoon at a time. To do this, we have used locks and condition variables and as per the results, 2 students can eat parallelly.
- We created 5 threads which have a conditional variable and wait on the canTakeSpoons function returning true. Further when sleeping the lock is given up so that other threads can run. It is locked again when operating on global variables like the student state.
- We chose to skip the "One Spoon Acquired" state because taking both spoons when they free is optimal than taking a spoon and waiting for other which can result in a deadlock when each student takes one spoon and waits for someone to give up a spoon.
- When a student is done eating and goes to thinking state, he gives up both spoons and goes to the waiting pool after the thinking period is over.
- When notifying other threads after completion, the scheduler schedules the thread that has waited the most and a particular student doesn't starve. We can confirm this from the state change log where against each student the cycles completed is printed and in most cases all students would have completed the same number of cycles.

### Problem 3 – Matrix Multiplication
- We are comparing the sequential program against 3 variants of parallelised matrix multiplication algorithms.
- A `mythreads.h` library has been included, which is basically a wrapper to some standard `pthread.h` library functions. The wrappers check for exception safety.
- **Variant 1** -
	- We create $n^2$ threads, each one computing one of the output element of the final product matrix.
- **Variant 2** -
	- We create $n$ threads, each one computing one row final product matrix.
- **Variant 3** -
	- We create 4 threads and use locks for computing the final product matrix.
	- Divide the first matrix into 4 quadrants. perform the sequential method with each quadrant with the 2nd matrix while holding a lock on the result of a partuicular element of the product matrix. 
	- The lock is needed as the other thread is also performing an addition on a shared variable result.
- Plots
	- *insert the plot of all the 4 variants (single graph)*
- Observations
	- To be added
