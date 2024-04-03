#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>

struct thread_data_t {
    // Pointer to the array
    int* arr_ptr;
    // Number of elements to be processed by thread
    int n;
} thread_data_t;

DWORD WINAPI square_elements(void* args) {
    struct thread_data_t* data = (struct thread_data_t*)args;
    for (int i = 0; i < data->n; ++i) {
        data->arr_ptr[i] = data->arr_ptr[i] * data->arr_ptr[i];
    }
    return 0;
}

int main(int argc, char** argv) {
    // Check if number of threads is passed
    if (argc < 2) {
        return 1;
    }
    int n = atoi(argv[1]); // NOLINT(*-err34-c)

    // Prepare data to be processed
    int m = 1024 * 1024 * 1024;
    int* data_to_process = malloc(m * sizeof(int));
    for (int i = 0; i < m; ++i) {
        data_to_process[i] = i;
    }

    printf("Data before processing:\n");
    for (int i = 10; i < 15; ++i) {
        printf("\t%i: %i\n", i, data_to_process[i]);
    }
    printf("\n");

    // Prepare thread handlers and thread data
    HANDLE* thread_handlers = malloc(n * sizeof(HANDLE));
    struct thread_data_t* thread_data = malloc(n * sizeof(struct thread_data_t));
    for (int i = 0; i < n; ++i) {
        thread_data[i].arr_ptr = data_to_process + (m / n)*i;
        thread_data[i].n = (m / n);
    }

    DWORD id;
    clock_t start_time = clock();
    for (int i = 0; i < n; ++i) {
        thread_handlers[i] = CreateThread(
                NULL,
                0,
                square_elements,
                (void*)&thread_data[i],
                0,
                &id);

        if (thread_handlers[i] != INVALID_HANDLE_VALUE) {
            printf("Created %i thread that calculates %i elements from 0x%llx\n", i, thread_data[i].n, (unsigned long long) thread_data[i].arr_ptr);
            SetThreadPriority(thread_handlers[i], THREAD_PRIORITY_NORMAL);
        }
    }

    WaitForMultipleObjects(n, thread_handlers, 1, INFINITE);
    clock_t end_time = clock();
    float processing_time_in_seconds = (float)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Calculation has finished. It took %f seconds\n", processing_time_in_seconds);

    printf("\nData after processing:\n");
    for (int i = 10; i < 15; ++i) {
        printf("\t%i: %i\n", i, data_to_process[i]);
    }
    printf("\n");

    free(thread_data);
    free(thread_handlers);
    free(data_to_process);
    return 0;
}
