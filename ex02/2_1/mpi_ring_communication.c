#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define ITERATIONS 1000

int main ( int argc , char * argv [] )
{
    int rank, StartFlag;
    int number_of_processes;
    char* bufferOut;
    char* bufferIn;
    double begin, end;
    long messageCount = 0;
    bufferOut = (char*) malloc(100 * sizeof(char));
    bufferIn = (char*) malloc(100 * sizeof(char));
    bufferOut = "Testnachricht";
    MPI_Status status;
    StartFlag = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // printf("hello from process %d of %d\n", rank, number_of_processes);

    MPI_Barrier(MPI_COMM_WORLD);

    if(rank == 0)
    {
        begin = MPI_Wtime();
    }

    int resciever =  ((rank + 1) % number_of_processes);
    int i;
    for(i = 0; i < ITERATIONS; i++)
    {
        if(rank == 0 && StartFlag == 0) // Master
        {
            MPI_Send(bufferOut, sizeof(bufferOut)/sizeof(char), MPI_CHAR, resciever, 0, MPI_COMM_WORLD);
            StartFlag = 1;
        }
        else{                           // Slave
            MPI_Recv(bufferIn, sizeof(bufferIn)/sizeof(char), MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            bufferOut = bufferIn;
            MPI_Send(bufferOut, sizeof(bufferOut)/sizeof(char), MPI_CHAR, resciever, 0, MPI_COMM_WORLD);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    
    if(rank == 0)
    {
        end = MPI_Wtime();
        double time_spent = ((double)(end - begin))*1000 ;
        double time_per_Roundtrip = ((double)(time_spent*1000)/ITERATIONS) ;
        double averageTimePerMess = ((double) (time_spent*1e6) / (number_of_processes*ITERATIONS));
        printf("N;%d;Runtime;%lf;ms;", number_of_processes, time_spent);
        printf("Roundtrip;%lf;ms;", time_per_Roundtrip);
        printf("Average;%lf;us", averageTimePerMess);
    }

    MPI_Finalize();
    return 0;
}