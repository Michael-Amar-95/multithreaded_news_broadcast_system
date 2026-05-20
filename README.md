# Concurrent News Broadcasting System

## Overview

This project implements a complete multithreaded news broadcasting simulation in C++.

The system models a real-world concurrent message pipeline in which multiple producers generate news reports, a dispatcher categorizes them, co-editors process the content, and a screen manager displays the final output.

The project focuses on:
- Concurrent programming
- Thread synchronization
- Producer-consumer architecture
- Bounded buffer implementation
- Semaphore-based synchronization
- Inter-thread communication

---

# System Architecture

The system contains four major active components:

1. Producers
2. Dispatcher
3. Co-Editors
4. Screen Manager

The architecture simulates a real-time news broadcasting workflow.

```text
Producers → Dispatcher → Co-Editors → Screen Manager
```

---

# Components

## Producers

Each producer generates news messages in the format:

```text
Producer <id> <type> <counter>
```

Example:

```text
Producer 2 SPORTS 0
Producer 2 WEATHER 1
```

Each producer:
- Runs in its own thread
- Produces a predefined number of messages
- Sends messages into a private bounded queue
- Sends a final `DONE` message after completion

Supported message types:
- SPORTS
- NEWS
- WEATHER

---

## Dispatcher

The dispatcher:
- Continuously scans producer queues
- Uses a Round Robin scheduling algorithm
- Sorts messages by category
- Forwards messages into dedicated queues

Dispatcher queues:
- Sports queue
- News queue
- Weather queue

The dispatcher is non-blocking while scanning queues.

After receiving `DONE` from all producers, the dispatcher forwards `DONE` messages to all co-editors.

---

## Co-Editors

Each co-editor handles one message category:
- SPORTS
- NEWS
- WEATHER

Responsibilities:
- Receive categorized messages
- Simulate editing delay (0.1 seconds)
- Forward edited messages to the shared output queue

When receiving `DONE`, the message is forwarded immediately.

---

## Screen Manager

The screen manager:
- Receives processed messages
- Prints them to standard output
- Tracks termination signals

After receiving three `DONE` messages, the system terminates gracefully.

---

# Synchronization Model

## Bounded Buffer

Producer queues and the shared co-editor queue are implemented as thread-safe bounded buffers.

Supported operations:

```cpp
insert(char* s)
remove()
```

---

## Synchronization Mechanisms

The project uses:
- Threads
- Binary semaphores (mutex)
- Counting semaphores
- Critical sections
- Blocking synchronization

The bounded buffer implementation follows the classic producer-consumer synchronization model.

---

# Configuration File

The system is configured using an external configuration file.

Example:

```text
PRODUCER 1
30
queue size = 5

PRODUCER 2
25
queue size = 3

PRODUCER 3
16
queue size = 30

Co-Editor queue size = 17
```

Compact format is also supported.

---

# Key Concepts Demonstrated

## Concurrent Programming
Multiple system components execute simultaneously using threads.

## Producer-Consumer Pattern
Producers generate messages while consumers process them asynchronously.

## Thread Synchronization
Shared resources are protected using semaphores and synchronization primitives.

## Bounded Buffers
Thread-safe queues prevent race conditions and overflow issues.

## Scheduling Algorithms
The dispatcher uses Round Robin scheduling to ensure fairness.

---

# Technologies Used

- C++
- POSIX Threads (pthreads)
- Semaphores
- Concurrent Programming
- Synchronization Mechanisms
- Producer-Consumer Architecture

---

# Learning Outcomes

This project demonstrates practical implementation of:
- Multithreaded system design
- Synchronization under concurrent workloads
- Safe communication between threads
- Queue management
- Parallel processing pipelines
- Real-time style event handling

---

# Example Output

```text
Producer 1 SPORTS 0
Producer 2 WEATHER 0
Producer 3 NEWS 0
Producer 1 SPORTS 1
...
DONE
```

---

# Conclusion

This project simulates a scalable concurrent broadcasting pipeline while demonstrating core operating systems and synchronization concepts.

The implementation emphasizes:
- Correct synchronization
- Efficient inter-thread communication
- Modular concurrent architecture
- Safe bounded-buffer management
