*This project has been created as part of the 42 curriculum by dtaylor-.*

# Codexion

## Description
**Codexion** is a concurrent programming project developed in C as part of the 42 curriculum. Inspired by classical synchronization problems like the *Dining Philosophers*, Codexion simulates a high-pressure co-working environment where multiple software developers ("coders") sit in a circular arrangement, competing for limited hardware resources (USB dongles) required to compile their quantum code.

Each coder transitions through three distinct operational phases:
1. **Compiling**: Requires acquiring two adjacent USB dongles (left and right).
2. **Debugging**: Conducted after compiling is complete and dongles are released.
3. **Refactoring**: Conducted after debugging, prior to attempting to acquire dongles for the next compile cycle.

The objective of the simulation is to coordinate thread execution and resource allocation to prevent **burnout** (starvation/deadline failure) while enforcing non-trivial constraints such as **dongle cooldown periods** and customizable scheduling policies (**FIFO** and **EDF**).

---

## Instructions

### Compilation
The project includes a `Makefile` adhering to standard 42 rules (`-Wall -Wextra -Werror -pthread`). To compile the binary, run:

```bash
make
```

This generates the executable named `codexion`.

Additional Makefile targets:
- `make clean`: Removes object files.
- `make fclean`: Removes object files and the `codexion` binary.
- `make re`: Performs a full re-compilation.

### Execution & Usage
Run the program with the following 8 mandatory arguments:

```bash
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
```

#### Parameter Breakdown:
* `number_of_coders`: Number of coders sitting around the table (and equal number of dongles).
* `time_to_burnout` *(ms)*: Maximum time allowed between the start of a coder's last compilation (or simulation start) and their next compilation start before burnout occurs.
* `time_to_compile` *(ms)*: Time required to compile (requires holding 2 dongles).
* `time_to_debug` *(ms)*: Time spent debugging after compilation.
* `time_to_refactor` *(ms)*: Time spent refactoring before re-entering the acquisition queue.
* `number_of_compiles_required`: Target compilation count per coder. Simulation terminates successfully once all coders reach this number.
* `dongle_cooldown` *(ms)*: Cooldown period during which a released dongle cannot be acquired by any coder.
* `scheduler`: Resource arbitration policy. Options: `fifo` or `edf`.

#### Example Command:
```bash
./codexion 4 800 200 200 200 5 50 edf
```

---

## Blocking Cases Handled

In multithreaded concurrent system designs, resource contention can lead to severe operational failures. Codexion addresses and mitigates the following concurrency challenges:

1. **Deadlock Prevention & Coffman's Conditions**:
   - *Hold and Wait / Circular Wait*: To prevent deadlock when coders request left and right dongles simultaneously, dongle acquisition order is strictly ordered by resource index (e.g., lower index dongle first) or managed via centralized arbitration queues. This breaks the circular wait condition.
2. **Starvation & Liveness under EDF/FIFO**:
   - Under **Earliest Deadline First (EDF)** scheduling, coders closest to burning out (`last_compile_start + time_to_burnout`) are given priority over dongle allocation.
   - Priority queues (min-heaps) ensure $O(\log N)$ lookup and guarantee that no thread is indefinitely starved while other threads acquire resources.
3. **Dongle Cooldown Enforcement**:
   - When a dongle is released, its state remains locked/unavailable until `dongle_cooldown` milliseconds elapse. Timestamps track availability to prevent premature acquisition during high contention.
4. **Precise Burnout Detection (< 10 ms)**:
   - A dedicated **Monitor Thread** continuously surveys coder deadlines using high-precision timestamps (`gettimeofday`). If a coder exceeds `time_to_burnout`, the monitor immediately flags simulation termination and logs the burnout within 10 ms of occurrence.
5. **Log Serialization & Race Conditions**:
   - Output interleaving is prevented by wrapping all status print statements (`printf`) with a global logging mutex (`pthread_mutex_t`), ensuring clean, single-line atomic logs.

---

## Thread Synchronization Mechanisms

The implementation relies on POSIX threading primitives to coordinate state across threads securely:

### Primitives Used
* `pthread_mutex_t`: Protects shared resources including individual dongle states, priority queues, coder status structures, and stdout printing.
* `pthread_cond_t`: Used in conjunction with condition waiting (`pthread_cond_wait` / `pthread_cond_timedwait`) to put threads to sleep while waiting for dongles to become free or for cooldowns to expire, preventing active CPU spinlocks.
* `Custom Min-Heap (Priority Queue)`: Built in C to maintain request ordering based on arrival time (FIFO) or target deadline (EDF).

### Synchronization Architecture
* **Coder Threads**: Each coder runs in a dedicated POSIX thread, cycling through compiling, debugging, and refactoring states. When requesting dongles, the thread registers its intent in the scheduler queue and waits on a condition variable until granted access.
* **Monitor Thread**: Runs independently from coder threads. It inspects coder state structures under mutex locks to check whether any thread has missed its burnout deadline or if all threads have completed `number_of_compiles_required`.
* **State Protection**: Shared state variables (such as `is_simulation_over`, `last_compile_time`, and dongle availability) are strictly guarded by mutexes to eliminate data races.

---

## Resources

### Documentation & References
* POSIX Threads Programming (`pthreads` manual pages & LLNL tutorials)
* *Operating System Concepts* by Silberschatz, Galvin, and Gagne (Concurrency & Deadlocks)
* Earliest Deadline First (EDF) Scheduling Algorithm Specification

### AI Usage & Acknowledgments
In accordance with the 42 AI guidelines:
* **Conceptual Verification**: AI tools were queried for architectural design feedback regarding priority queue structures (min-heap implementation in C89) and condition variable broadcast mechanics.
* **Documentation & Formatting**: AI assisted in drafting, structuring, and formatting markdown elements for this `README.md` file to ensure full compliance with specification guidelines.
* **Code Validation**: All C implementation logic, mutex locking hierarchies, and synchronization boundaries were manually written, tested, and verified by the author.
