#include <stdio.h>
#include <unistd.h>

void *memorypool;

void *myallocate(int n) {
    return sbrk(n);
}

void initmemorypool(void) {
    memorypool = sbrk(0);
}

void resetmemorypool(void) {
    brk(memorypool);
}

int main() {
    initmemorypool();

    int *array = (int *)myallocate(10 * sizeof(int));

    if (array == (void *)-1) {
        puts("Falha na alocação de memória");
        return 1;
    }

    for (int i = 0; i < 10; i++) {
        array[i] = i * 10;
    }

    puts("Valores no array alocado:");
    for (int i = 0; i < 10; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");

    resetmemorypool();

    return 0;
}
