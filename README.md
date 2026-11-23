*This project has been created as part of the 42 curriculum by rbartok.*


## Description

Codexion is a multithreaded C simulation in which multiple coders compete for shared dongles to perform compilation cycles. Each coder must acquire two adjacent dongles before compiling, then proceeds through debugging and refactoring before requesting resources again.

The project focuses on concurrent resource management, synchronization, scheduling, timing, and termination. It implements FIFO and EDF scheduling policies while handling resource cooldowns, burnout detection, starvation prevention, and deadlock avoidance using POSIX threads and mutexes.


## Instructions

The program can be compiled with the **make command**, that creates output files for each C file, and an executable file named codexion.


Other make commands:
- make re : recompiles all the files
- make clean : delets the output files
- make fclean : deletes all the output files *and* the executable file

Running the code should be with the following parameters:
``
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
``

Example for running a succesful simulation:

``
./codexion 3 10000 500 300 200 3 100 fifo
``

## Resources
- [The Dining Philosophers Problem youtube video](https://www.youtube.com/watch?v=FYUi-u7UWgw)
- [mutexes 101 youtube tutorial](https://www.youtube.com/watch?v=raLCgPK-Igc)
- [posix threads youtube tutorial](https://www.youtube.com/watch?v=ldJ8WGZVXZk)
- [basic example article for mutexes and pthreads](https://www.thegeekstuff.com/2012/05/c-mutex-examples/)


For learning and roadmap creation I used AI, to get an actionable plan of execution. The following learning curve and todo plan was followed:

1. Create peoject file structure and handle argument validation
2. Create configuration struct and fill up based on the arguments
3. Learn about system time concepts and decide between using relavive vs absolute timestamps in the project
4. Create basic coder and dongle struct with their respective threads and mutexes
5. Add the functionality of compile, debug and refactor
6. Create basic coder lifecycle
7. Add scheduler logic to the process
8. Extend with simulation struct that synchronizes the whole process together
9. Add termination conditions and extend mutex protection to shared resources

## Blocking cases handled

The Codexion simulation uses multiple worker threads that compete for shared dongles while following a scheduler and a set of timing constraints. Several concurrency problems have to be handled explicitly to keep the simulation correct.

### Deadlock prevention

The main deadlock risk comes from coders needing **two adjacent dongles** to compile. Acquiring the dongles one at a time could create a circular wait: for example, one coder could hold the first dongle while waiting for the second, while another coder holds the second dongle while waiting for the first.

To prevent this, the implementation never partially acquires the required resources. `acquire_dongles()` locks the shared `dongle_mutex`, checks the availability and cooldown state of **both required dongles**, and, only if both are available, marks both as `IN_USE` before releasing the mutex.

Therefore, a coder cannot hold one dongle while waiting for another. This removes the **hold-and-wait** condition and consequently prevents the circular-wait scenario described by the Coffman deadlock conditions.

The same mutex protects the release operation. Both dongles are transitioned to `COOLDOWN` while holding `dongle_mutex`, so another worker cannot observe or modify a partially released resource state.

### Starvation prevention

The scheduler prevents coders from competing purely based on thread scheduling or which worker happens to retry first.

When a coder needs to compile, it enters the `REQUESTING` state and records a `request_time`. The timestamp is assigned only when the coder first enters `REQUESTING`, and is preserved while it continues waiting.

Rather than scanning every coder to find the best candidate, requesting coders are kept in a **binary min-heap** (`t_heap`), one per simulation, ordered by `has_higher_priority()`. A coder is pushed onto the heap exactly once, in `add_request_time_to_coder()`, the moment it transitions into `REQUESTING`.

The FIFO scheduler orders the heap by `request_time`, so the root is always the coder that has been waiting the longest. This prevents a coder’s position in the queue from being reset every time it retries acquiring the dongles.

The EDF scheduler orders the heap by `last_compile_start` instead. Since `time_to_burnout` is the same for every coder, the coder with the smallest `last_compile_start` is the one closest to its burnout deadline, i.e. the one with the earliest deadline.

Every waiting worker thread repeatedly asks “am I next?” by peeking the heap’s root with `heap_peek()` (`get_next_coder()`), an O(1), non-destructive read. Peeking rather than popping matters here: many coder threads consult the heap concurrently while only retrying, so removing an entry just to check it would let one thread steal another’s queue position. A coder is only actually removed from the heap, via `heap_pop()`, once it has been confirmed as the root **and** both of its dongles are available, i.e. once it is truly leaving the `REQUESTING` state. Push, peek and pop are each O(log n) or O(1), and are additionally serialized by the single global `dongle_mutex` held for the whole acquisition attempt, so the heap never needs to be reasoned about under concurrent modification.

### Dongle cooldown handling

A dongle does not become immediately reusable when a compilation finishes. When `release_dongles()` is called, both dongles are changed to `COOLDOWN` and receive an `unavailable_until` timestamp.

Cooldown expiration is checked while holding `dongle_mutex`. Expired cooldowns are transitioned back to `AVAILABLE` before a new acquisition is accepted.

This prevents a worker from acquiring a dongle during its mandatory cooldown period and also ensures that checking the cooldown and claiming the resource cannot race with another worker.

The cooldown state is therefore treated as part of the shared resource state rather than being implemented as a delay in an individual worker thread.

### Precise burnout detection

Burnout detection is based on timestamps relative to the common simulation start time rather than on the number of worker-loop iterations or the time at which a worker happens to check its state.

The coder's relevant timestamps, such as `last_compile_start`, are stored relative to `simulation_start_time`. This allows the simulation to determine whether the configured `time_to_burnout` has actually elapsed.

The simulation-finished state is also shared between the worker threads and the monitoring/termination logic. Workers check this state while waiting for resources and before starting additional work. This prevents a worker that has been blocked waiting for dongles from continuing to perform work after the simulation has already terminated.

The simulation also waits for the worker threads to finish before destroying shared simulation resources, preventing workers from accessing freed mutexes, coders, dongles, or configuration data.

### State synchronization

Coder state is shared between worker threads and the scheduler, so state changes cannot be treated as ordinary unsynchronized variables.

For example, `add_request_time_to_coder()` protects the transition into `REQUESTING` with `coder_mutex`. It only changes the state and records the request timestamp when the coder was not already requesting. This is important because repeated acquisition attempts must not continually replace the original request timestamp.

Consequently, the sequence is conceptually:

`CHILLING → REQUESTING → COMPILING → DEBUGGING → REFACTORING → CHILLING`

with the `REQUESTING` timestamp representing when the coder actually entered the scheduling queue.

### Atomic dongle acquisition and release

The check-and-modify operation for dongles is performed atomically with respect to other workers.

A worker does not:

1. check whether dongle A is available,
2. release the mutex,
3. check dongle B,
4. and then acquire them separately.

Instead, both required dongles are checked and claimed while `dongle_mutex` is held. This prevents another worker from changing the resource state between the availability check and the acquisition.

The same principle is used when releasing the two dongles: their states and cooldown timestamps are updated while the shared mutex is held.


## Thread syncronization mechanisms

In my project I have used 2 separate threads. One for the coders (***coder thread***), each coder as the subject requires is represented by a thread. These threads are then joined and their functionality is handled by the worker. The worker has all of the coders lifecycle (compile, debug, refactor). The other thread is the ***simulation thread***, that has the purpose of monitoring the simulation, and any time the coders have all successfully compiled the necesarry amount, or in case of a coder burnout it immediately stops the program. There are several shared rescources/data in the program that are accessed in both threads, so I use 3 mutexes. The whole dongle usage is wrapped around a ***dongle_mutex***, dongles being the central shared rescource in the program. I also have a ***simulation_mutex*** that is protecting the simulation_finished flag that is read by both threads, to check when the simulation should end. 
The third is the ***coder_mutex*** that is used to protect the coder state, compilation_count and last_compile_start variables, since these are checked in the simulation thread as well, being factors in deciding when the simulation should end.

All three mutexes live inside the simulation struct, that oversees the whole program:

```
typedef struct s_simulation
{
	t_config		*config;
	t_coder			*coders;
	t_dongle		*dongles;
	long long		simulation_start_time;
	int				simulation_finished;
	pthread_mutex_t	dongle_mutex;
	pthread_mutex_t	simulation_mutex;
	pthread_mutex_t	coder_mutex;
	pthread_t		thread;
	t_heap			heap;
}	t_simulation;
```