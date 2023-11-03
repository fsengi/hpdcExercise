#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define MIN_MSG_SIZE 1024    // 1KB
#define MAX_MSG_SIZE 1048576 // 1MB
#define ITERATIONS 1000

int main(int argc, char *argv[])
{
    int rank, size;
    int msg_size;
    char *send_buffer, *recv_buffer;
    double begin, end, roundtrip_latency;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2)
    {
        if (rank == 0)
            printf("This program requires exactly 2 processes.\n");
        MPI_Finalize();
        return 1;
    }

    send_buffer = (char *)malloc(MAX_MSG_SIZE * sizeof(char));
    recv_buffer = (char *)malloc(MAX_MSG_SIZE * sizeof(char));

    for (msg_size = MIN_MSG_SIZE; msg_size <= MAX_MSG_SIZE; msg_size *= 2)
    {
        begin = MPI_Wtime();

        int i;
        for (i = 0; i < ITERATIONS; i++)
        {
            if (rank == 0)
            {
                MPI_Send(send_buffer, msg_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
                MPI_Recv(recv_buffer, msg_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            else
            {
                MPI_Recv(recv_buffer, msg_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(send_buffer, msg_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }
        }

        end = MPI_Wtime();

        roundtrip_latency = (end - begin) / (2 * ITERATIONS) * 1e6; // Convert to microseconds

        if (rank == 0)
            printf("Message Size: %d bytes, Latency: %lf microseconds\n", msg_size, roundtrip_latency);
    }

    free(send_buffer);
    free(recv_buffer);

    MPI_Finalize();
    return 0;
}
