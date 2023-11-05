#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Request request = MPI_REQUEST_NULL;

    if (size < 2) {
        fprintf(stderr, "This program requires at least 2 MPI processes.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Define message sizes (in bytes) to test
    int message_sizes[] = {1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576};
    int num_sizes = sizeof(message_sizes) / sizeof(int);

    for (int s = 0; s < num_sizes; s++) {
        int message_size = message_sizes[s];
        char* message = (char*)malloc(message_size);
        
        double start_time, end_time;
        double elapsed_time;

        MPI_Barrier(MPI_COMM_WORLD); // Synchronize processes

        if (rank == 0) {
            start_time = MPI_Wtime(); // Start the timer
            MPI_Isend(message, message_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, MPI_STATUS_IGNORE);
            end_time = MPI_Wtime(); // Stop the timer
            elapsed_time = end_time - start_time;

            double bandwidth = (message_size / 1024.0 / 1024.0) / elapsed_time; // in MB/s
            printf("Message size: %d bytes, Bandwidth: %f MB/s\n", message_size, bandwidth);
        } else if (rank == 1) {
            MPI_Recv(message, message_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        free(message);
    }

    MPI_Finalize();
    return 0;
}
