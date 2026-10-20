#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main(int argc, char *argv[])
{
    int p1, p2, p3, status;
    if (argc != 2) {
        printf("Usage: %s <file.c>\n", argv[0]);
        exit(1);
    }
    /* ---------- P1 : compilation (gcc -c file.c -o prog1.o) ---------- */
    p1 = fork();
    if (p1 == -1) { perror("fork P1"); exit(2); }
    if (p1 == 0) { /* child P1 only */
        execlp("gcc", "gcc", "-c", argv[1], "-o", "prog1.o", NULL);
        perror("execlp gcc (P1)"); /* reached only if exec failed */
        exit(127);
    }
    wait(&status); /* parent waits for P1 */
    if (!(WIFEXITED(status) && WEXITSTATUS(status) == 0)) {
        printf("P1 failed: compilation error\n");
        exit(3);
    }
    printf("P1 finished successfully: prog1.o created\n");
    /* ---------- P2 : link editing (gcc prog1.o -o prog1) ---------- */
    p2 = fork();
    if (p2 == -1) { perror("fork P2"); exit(2); }
    if (p2 == 0) { /* child P2 only */
        execlp("gcc", "gcc", "prog1.o", "-o", "prog1", NULL);
        perror("execlp gcc (P2)");
        exit(127);
    }
    wait(&status); /* parent waits for P2 */
    if (!(WIFEXITED(status) && WEXITSTATUS(status) == 0)) {
        printf("P2 failed: link edition error\n");
        exit(4);
    }
    printf("P2 finished successfully: prog1 created\n");
    /* ---------- P3 : execution of ./prog1 ---------- */
    p3 = fork();
    if (p3 == -1) { perror("fork P3"); exit(2); }
    if (p3 == 0) { /* child P3 only */
        execl("./prog1", "prog1", NULL);
        perror("execl ./prog1");
        exit(127);
    }
    /* ---------- Parent analyses the termination of P3 ---------- */
    wait(&status);
    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) == 0)
            printf("P3: normal termination, without error\n");
        else
            printf("P3: normal termination, with error (code %d)\n",
                   WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("P3: abnormal termination, killed by signal %d (%s)%s\n",
               WTERMSIG(status), strsignal(WTERMSIG(status)),
               WCOREDUMP(status) ? " - core dumped" : "");
    }
    return 0;
}
