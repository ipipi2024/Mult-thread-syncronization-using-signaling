# Multi-thread Synchronization Using Signaling

This project implements classic synchronization problems using POSIX semaphores and threads in C++.

## Problems Implemented

### Problem 1: No-Starve Readers-Writers

A solution to the readers-writers problem that prevents writer starvation. This implementation uses three semaphores to ensure fair access:

- **turnstile**: Acts as a "door" that writers can close to prevent new readers from entering
- **roomEmpty**: Indicates whether the critical section is empty (no readers inside)
- **mutex_r**: Protects the reader count variable

#### How It Works

**Readers:**
1. Pass through the turnstile (immediately signal it back)
2. Use a lightswitch pattern to track reader count
3. First reader locks roomEmpty, last reader unlocks it
4. Read the shared data

**Writers:**
1. Lock the turnstile (preventing new readers)
2. Wait for the room to be empty (all current readers finish)
3. Write to shared data
4. Release both roomEmpty and turnstile

This ensures writers don't starve since they can block new readers from entering while waiting for current readers to finish.

#### Example Output

![No-Starve Readers-Writers Output](no-starving.png)

The output shows readers and writers accessing shared data in a synchronized manner, with writers incrementing the value and readers reading it.

## Building and Running

### Compilation

```bash
make
```

Or manually:
```bash
g++ -g main.cpp -o cse4001_sync -lpthread -lm
```

### Execution

```bash
./cse4001_sync 1
```

Where `1` specifies Problem 1 (No-Starve Readers-Writers).

### Cleaning

```bash
make clean
```

## Code Structure

- `main.cpp` - Main implementation with reader/writer threads and problem logic
- `semaphore_class.h` - Semaphore wrapper class for POSIX semaphores
- `makefile` - Build configuration

## Requirements

- C++ compiler with C++11 support
- POSIX threads library (pthread)
- POSIX semaphores (semaphore.h)

## Implementation Details

The solution uses:
- 5 reader threads
- 5 writer threads
- Shared integer variable that writers increment and readers read
- Semaphore-based synchronization to prevent race conditions and starvation
