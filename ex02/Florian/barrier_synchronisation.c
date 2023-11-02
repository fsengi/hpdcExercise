#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

void MPI_Barrier_own(MPI_Comm com);


void MPI_Barrier_own(MPI_Comm MPI_Communicator)
{
    int rank, number_of_processes;
    char bufferOut[10] = "!";
    char bufferIn[10] = "";
    int tag = 130;
    MPI_Status status;

    MPI_Comm_size(MPI_Communicator, &number_of_processes);
    MPI_Comm_rank(MPI_Communicator, &rank);

    if(rank == 0)                   // Master
    {   
        int reciever;
        for(reciever = 1; reciever < number_of_processes; reciever++)
        {
            MPI_Send(bufferOut, sizeof(bufferOut)/sizeof(char), MPI_CHAR, reciever, tag, MPI_Communicator);
        }
    }
    else                            // Slave
    {                          
        MPI_Recv(bufferIn, sizeof(bufferIn)/sizeof(char), MPI_CHAR, 0, tag, MPI_Communicator, &status);
    }
}

int main ( int argc , char * argv [] )
{
    // printf("argc: %d argv: %s", argc, *argv);fflush(stdout);
    int iterations = atoi(argv[1]);
    // int iterations = 1000;
    int number_of_processes, rank;
    double begin_own, end_own, begin, end;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Barrier(MPI_COMM_WORLD);

    if(rank == 0)
    {
        begin = MPI_Wtime();
    }
    int iter_own;
    for(iter_own = 0; iter_own < iterations; iter_own++)
    {
        MPI_Barrier(MPI_COMM_WORLD);
    }
    if(rank == 0)
    {
        end = MPI_Wtime();
        begin_own = MPI_Wtime();
    }
    int iter;
    for(iter = 0; iter < iterations; iter++)
    {
       MPI_Barrier_own(MPI_COMM_WORLD);
    }
    
    if(rank == 0)
    {
        end_own = MPI_Wtime();
        double time_spent_own = ((double)(end_own - begin_own))*1e6/iterations ;
        double time_spent = ((double)(end - begin))*1e6/iterations ;
        printf("N;%d;Runtime own Barrier;%lf;us;Runtime MPI Barrier;%lf;us;iterations;%d\n", number_of_processes, time_spent_own, time_spent, iterations);fflush(stdout);
        MPI_Finalize();
    }
    return 0;
}
