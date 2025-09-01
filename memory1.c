#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE (100 * 1024 * 1024)  // 100 MB

int main() {
    char *arr = malloc(SIZE);  // allocate a big block
    if (!arr) {
        perror("malloc failed");
        return 1;
    }

    printf("Allocated %d bytes. PID = %d\n", SIZE, getpid());
    sleep(1000);  // pause to let us inspect with ps/top
    free(arr);
    return 0;
}
