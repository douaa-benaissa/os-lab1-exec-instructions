#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(void)
{
    /* 0 = OK, 1 = exit(3), 2 = SIGSEGV, 3 = division by zero, 4 = sleep 60s */
    int scenario = getenv("SCENARIO") ? atoi(getenv("SCENARIO")) : 0;
    volatile int zero = 0;
    int *p = NULL;
    printf("Hello from prog1 (scenario %d)\n", scenario);
    switch (scenario) {
        case 1: exit(3);
        case 2: *p = 1; break;
        case 3: printf("%d\n", 10 / zero); break;
        case 4: sleep(60); break;
    }
    return 0;
}
