# syc-os-lab1-exec-instructions
# OS Lab: Unix processes and the `exec` family

Lab for the course **Centralized Operating System**, Chapter 2 (Processes).

## What the program does

A parent process creates three children, one after another. Each child
replaces itself with another program using `exec`:

| Child | Command executed | Created when |
|---|---|---|
| **P1** | `gcc -c <file.c> -o prog1.o` (compilation) | at the start |
| **P2** | `gcc prog1.o -o prog1` (link editing) | P1 ended normally with code 0 |
| **P3** | `./prog1` (execution) | P2 ended normally with code 0 |

The parent calls `wait()` after each child and analyses the status with the
macros `WIFEXITED`, `WEXITSTATUS`, `WIFSIGNALED`, `WTERMSIG`, `WCOREDUMP`.
For P3 it reports one of three cases:

1. normal termination, without error (code 0)
2. normal termination, with error (non-zero code, printed)
3. abnormal termination: killed by a signal (number and name printed)

## Concepts used

`fork()`, `execl` / `execlp`, `wait()`, exit codes, signals, the status
word returned by `wait`.

## Requirements

Linux / Unix only (uses `fork`, `exec`, `wait`). Tested on Ubuntu.
It does **not** compile on native Windows: use WSL, a Linux VM or a
GitHub Codespace.

## Safety note

The program compiles and then **executes** the C file given as argument.
Only run it on files you trust.

## Repository layout

```
src/lab_exec.c    the lab program (parent + P1, P2, P3)
tests/prog1.c     test program run by P3, scenario chosen by SCENARIO
```

## Build and run

From the repository root:

```
gcc -Wall -o a.out src/lab_exec.c
./a.out tests/prog1.c
```

`prog1.o` and `prog1` are created in the current directory (they are in
`.gitignore`).

## Test scenarios

`tests/prog1.c` reads the environment variable `SCENARIO` (the child P3
inherits the environment of its parent):

| Command | What prog1 does | Expected P3 report |
|---|---|---|
| `./a.out tests/prog1.c` | returns 0 | normal termination, without error |
| `SCENARIO=1 ./a.out tests/prog1.c` | `exit(3)` | normal termination, with error (code 3) |
| `SCENARIO=2 ./a.out tests/prog1.c` | writes to a NULL pointer | killed by signal 11 (Segmentation fault) |
| `SCENARIO=3 ./a.out tests/prog1.c` | division by zero | killed by signal 8 (Floating point exception) |
| `SCENARIO=4 ./a.out tests/prog1.c` | sleeps 60 s | run it, then `kill <PID>` or `kill -9 <PID>` from a second terminal: signal 15 or 9 |

Do not use Ctrl+C for scenario 4: the signal goes to the whole foreground
group and also kills the parent.

## Sample output (terminal)

```
$ ./a.out tests/prog1.c
P1 finished successfully: prog1.o created
P2 finished successfully: prog1 created
Hello from prog1 (scenario 0)
P3: normal termination, without error

$ SCENARIO=1 ./a.out tests/prog1.c
P1 finished successfully: prog1.o created
P2 finished successfully: prog1 created
Hello from prog1 (scenario 1)
P3: normal termination, with error (code 3)

$ SCENARIO=2 ./a.out tests/prog1.c
P1 finished successfully: prog1.o created
P2 finished successfully: prog1 created
Hello from prog1 (scenario 2)
P3: abnormal termination, killed by signal 11 (Segmentation fault)
```

## Exit codes of the main program

| Code | Meaning |
|---|---|
| 0 | all three steps ran (P3's result is printed) |
| 1 | wrong number of arguments |
| 2 | `fork` failed |
| 3 | P1 (compilation) failed |
| 4 | P2 (link editing) failed |

Inside a child, exit code 127 means that `exec` itself failed.
