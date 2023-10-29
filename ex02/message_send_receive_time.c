#include <mpi.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, const char** argv) {
    MPI_Init(&argc, (char***)&argv);

    int comm_size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    uint8_t data = 0;
    MPI_Status status;
    int nummsg = atoi(argv[1]); // Assuming you pass the value for nummsg as a command-line argument

    struct timespec starttime, endtime;
    clock_gettime(CLOCK_MONOTONIC, &starttime);
    
    int i;
    for (i = 0; i < nummsg; i++) {
        MPI_Send(&data, 0, MPI_BYTE, (rank + 1) % comm_size, 0, MPI_COMM_WORLD);
        MPI_Recv(&data, 1, MPI_BYTE, (rank - 1 + comm_size) % comm_size, 0, MPI_COMM_WORLD, &status);
    }

    clock_gettime(CLOCK_MONOTONIC, &endtime);

    double elapsed_time = (endtime.tv_sec - starttime.tv_sec) + (endtime.tv_nsec - starttime.tv_nsec) / 1e9;

    printf("Process %d finished in %f seconds.\n", rank, elapsed_time);

    MPI_Finalize();
    return 0;
}
