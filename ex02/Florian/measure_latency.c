#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

void MPI_Barrier_own(MPI_Comm MPi_CommVar);

int main (int argc, char * argv [] )
{
    int iterations = argc;
    int rank;
    int number_of_processes;
    double begin, end;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // printf("hello from process %d of %d\n", rank, number_of_processes);

    MPI_Barrier(MPI_COMM_WORLD);

    if(rank == 0)
    {
        begin = MPI_Wtime();
    }
    for(int i = 0; i < iterations; i++)
    {
        MPI_Barrier_own(MPI_COMM_WORLD);
    }
    if(rank == 0)
    {
        end = MPI_Wtime();
        double time_spent = ((double)(end - begin))*1000 ;
        double averageTimePerMess = ((double) (time_spent*1e6) / (number_of_processes*iterations));
        printf("N;%d;Runtime;%lf;ms;", number_of_processes, time_spent);
        printf("Average;%lf;us", averageTimePerMess);
    }
    MPI_Finalize();
    return 0;
}


void MPI_Barrier_own(MPI_Comm MPi_CommVar)
{
    int num_process, rank;
    char* buffer[2];
    *buffer = "!";
    int tag = 30;
    MPI_Status status;

    MPI_Comm_size(MPI_COMM_WORLD, &num_process);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if(rank == 0)
    {
        for(int i = 0; i <= num_process; i++)
        {
            MPI_Send(buffer, sizeof(buffer)/sizeof(char), MPI_CHAR, i, tag, MPI_COMM_WORLD);
            MPI_Recv(buffer, sizeof(buffer)/sizeof(char), MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
        }
    }
}