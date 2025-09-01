#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE (100 * 1024 * 1024)  // 100 MB

int main() {
    char *arr = malloc(SIZE);
    if (!arr) {
        perror("malloc failed");
        return 1;
    }

    // Touch every page of the array so OS must back it with real RAM
    for (int i = 0; i < SIZE; i += 4096) {
        arr[i] = 0;
    }

    printf("Allocated and touched %d bytes. PID = %d\n", SIZE, getpid());
    sleep(1000);
    free(arr);
    return 0;
}
