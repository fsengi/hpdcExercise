#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <mpi.h>
#include <stdbool.h>
#include <string.h>

#define N 10
#define OUTLAY 1
#define TIME 10
#define PHI 0.24
#define GIFFLAG 0
// #define DEBUG

void printGrid(double *grid, int size);
void printSubGrid(double *grid, int size, int rows, int rank);
void printSubGridName(char* name, double *grid, int size, int rows, int rank, int time);
double measureTime(int size, int timeIter);

int main(int argc ,char * argv[]) {
    int size, time, timeIter, number_of_processes, gifFlag, rank = 0;
    int row, col;
    double *gridNew, *gridOld, avg_time;
    struct timeval start, end;
    // Declare our neighbours
    enum DIRECTIONS {DOWN, UP};
    char* neighbours_names[2] = {"down", "up"};
    int neighbours_ranks[2];
    MPI_Status status;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Ask MPI to decompose our processes in a 2D cartesian grid for us
    int dims[1] = {number_of_processes};
    MPI_Dims_create(number_of_processes, 1, dims);

    // Make both dimensions non-periodic
    int periods[1] = {0};

    // Let MPI assign arbitrary ranks if it deems it necessary
    int reorder = true;

    // Create a communicator with a cartesian topology.
    MPI_Comm new_communicator;
    MPI_Cart_create(MPI_COMM_WORLD, 1, dims, periods, reorder, &new_communicator);

    // Let consider dims[1] = Y, so the shift tells us our up and down neighbours
    MPI_Cart_shift(new_communicator, 1, 1, &neighbours_ranks[DOWN], &neighbours_ranks[UP]);

    // Get my rank in the new communicator
    MPI_Comm_rank(new_communicator, &rank);

#ifdef DEBUG
    printf("Rank: %d, neighbours_ranks[UP]: %d, neighbours_ranks[DOWN]: %d\n", rank, neighbours_ranks[UP], neighbours_ranks[DOWN]);
#endif

    if(rank == 0)
    {
        // get args
        if (argc > 3) {
            size = atoi(argv[1]);  // Parse size from command line argument
            timeIter = atoi(argv[2]);
            gifFlag = atoi(argv[3]);
        } else {
            size = N;
            timeIter = TIME;
            gifFlag = GIFFLAG;
        }

        // allocate grid
        gridOld = (double *)malloc(size * size * sizeof(double));
        gridNew = (double *)malloc(size * size * sizeof(double));

        // initialize
        for (col = 0; col < size; col++) 
        {
            if (col >= size / 4 && col <= 3 * size / 4) {
                gridOld[col] = 127;
            }
        }

        // printGrid(gridOld,size);

        int i;
        for(i = 1; i < number_of_processes; i++)
        {
            MPI_Send(&size, sizeof(size)/sizeof(int), MPI_INT, i, 1, new_communicator);
            MPI_Send(&timeIter, sizeof(timeIter)/sizeof(int), MPI_INT, i, 2, new_communicator);
        }
    }
    else
    {
        MPI_Recv(&size, sizeof(size)/sizeof(int), MPI_INT, MPI_ANY_SOURCE, 1, new_communicator, &status);
        MPI_Recv(&timeIter, sizeof(timeIter)/sizeof(int), MPI_INT, MPI_ANY_SOURCE, 2, new_communicator, &status);
    }
    int rowsPerProcess = size / number_of_processes;
    int diff = size - (rowsPerProcess * number_of_processes); 
    int counts[number_of_processes];
    int displacement[number_of_processes];

    int j;
    for(j = 0; j < number_of_processes; j++)
    {
        if(j < number_of_processes - 1)
        {
            counts[j] = rowsPerProcess * size;
            displacement[j] = rowsPerProcess * size * j;
        }
        else
        {
            counts[j] = (rowsPerProcess + diff) * size;
            displacement[j] = rowsPerProcess * size * j;
        }
#ifdef DEBUG
        if(rank == 0)
        {
            printf("i: %d, counts: %d displacement: %d rowsPerProcess: %d diff: %d\n", j, counts[j], displacement[j], rowsPerProcess, diff);
        }
#endif
    }
    
    if(rank == number_of_processes-1)
    {   
        rowsPerProcess = rowsPerProcess + diff;
    }
#ifdef DEBUG
    printf("rank: %d rowsPerProcess: %d\n", rank, rowsPerProcess);
#endif
    double *subGridOld = (double *)malloc(rowsPerProcess * size * sizeof(double));
    double *subGridNew = (double *)malloc(rowsPerProcess * size * sizeof(double));
    
    double *subGridUpOld = (double *)malloc(size * sizeof(double));
    double *subGridDownOld = (double *)malloc(size * sizeof(double));
    double *subGridUpNew = (double *)malloc(size * sizeof(double));
    double *subGridDownNew = (double *)malloc(size * sizeof(double));
    
    MPI_Scatter(gridOld, size * rowsPerProcess, MPI_DOUBLE, subGridOld, size * rowsPerProcess, MPI_DOUBLE, 0, new_communicator);

    for (time = 0; time < timeIter; time++) 
    {
        for (row = 0; row < rowsPerProcess; row++) 
        {
            for (col = OUTLAY; col < size - OUTLAY; col++) 
            {
                double upVal, downVal = 0;
                if((rank == 0 && row == 0) || (rank == number_of_processes-1 && row == rowsPerProcess-1))
                {
                    subGridNew[row * size + col] = subGridOld[row * size + col];
                    continue;
                }
                else if (row == 0)
                {
                    upVal = subGridUpNew[col];
                    downVal = subGridOld[((row + 1) * size) + col];
                }
                else if (row == rowsPerProcess-1)
                {
                    upVal = subGridOld[((row - 1) * size) + col];
                    downVal = subGridDownNew[col];
                }
                else
                {
                    upVal = subGridOld[((row + 1) * size) + col];
                    downVal = subGridOld[((row - 1) * size) + col];
                }
                subGridNew[row * size + col] = subGridOld[row * size + col] +
                                             PHI * ((-4.0) * subGridOld[row * size + col] +
                                                    upVal +
                                                    downVal +
                                                    subGridOld[(row * size) + col + 1] +
                                                    subGridOld[(row * size) + col - 1]);
            }
        }

        memcpy(subGridUpOld, &subGridNew, size*sizeof(double));
        memcpy(subGridDownOld, &subGridNew[(rowsPerProcess-1)*size], size*sizeof(double));
       
        if(rank == 0)
        { 
            MPI_Request request;
            
            MPI_Isend(subGridDownOld, size, MPI_DOUBLE, neighbours_ranks[UP], 0, new_communicator, &request);
            MPI_Recv(subGridDownNew, size, MPI_DOUBLE, neighbours_ranks[UP], 0, new_communicator, &status);
        }
        else if (rank == number_of_processes-1)
        {
            MPI_Request request;
            MPI_Isend(subGridUpOld, size, MPI_DOUBLE, neighbours_ranks[DOWN], 0, new_communicator, &request);
            MPI_Recv(subGridUpNew, size, MPI_DOUBLE, neighbours_ranks[DOWN], 0, new_communicator, &status);
        }
        else
        {
            
            MPI_Request request;
            MPI_Isend(subGridDownOld, size, MPI_DOUBLE, neighbours_ranks[UP], 0, new_communicator, &request);
            MPI_Recv(subGridDownNew, size, MPI_DOUBLE, neighbours_ranks[UP], 0, new_communicator, &status);

            MPI_Isend(subGridUpOld, size, MPI_DOUBLE, neighbours_ranks[DOWN], 0, new_communicator, &request);
            MPI_Recv(subGridUpNew, size, MPI_DOUBLE, neighbours_ranks[DOWN], 0, new_communicator, &status);
        }
        MPI_Barrier(new_communicator);

        if(gifFlag == 1 || time == timeIter-2)
        {
            MPI_Gatherv(subGridNew, counts[rank], MPI_DOUBLE, (rank==0?gridNew:NULL), (rank==0?counts:NULL), (rank==0?displacement:NULL), MPI_DOUBLE, 0, new_communicator);
            if(rank == 0)
            {
                // printGrid(gridNew, size);
            }
        }
        // Swap grids

        double *temp = subGridOld;
        subGridOld = subGridNew;
        subGridNew = temp;
    }

    // MPI_Barrier(new_communicator);

    //  MPI_Gatherv(subGridNew, counts[rank], MPI_DOUBLE, (rank==0?gridNew:NULL), (rank==0?counts:NULL), (rank==0?displacement:NULL), MPI_DOUBLE, 0, new_communicator);

    // if(rank == 0)
    // {
    //     printGrid(gridNew, size);
    // }

    // gettimeofday(&start, NULL);

    // // Calculate average time dynamically


    //         }
    //     }
    // }

    // gettimeofday(&end, NULL);

    // // Free allocated memory
    // free(gridOld);
    // free(gridNew);

    // double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

    // avg_time = elapsed_time / timeIter;

    // // Calculate FLOPs per iteration
    // double flops_per_iteration = 7.0 * size * size;

    // // Calculate total FLOPs for the current size
    // double total_flops = flops_per_iteration * timeIter;

    // // Calculate GFLOP/s
    // double gflops = flops_per_iteration / (avg_time * 1e9);

    // printf("%dx%d\t\t%.6lf\t\t%.0lf\t\t%.3lf\n", size, size, avg_time, total_flops, gflops);
    if(rank == 0)
    {
        printf("size: %d, np: %d complete\n", size, number_of_processes);
    }
    MPI_Finalize();
    free(gridNew);
    free(gridOld);
    free(subGridDownNew);
    free(subGridDownOld);
    free(subGridUpNew);
    free(subGridUpOld);

    return 0;
}

void printGrid(double *grid, int size)
{
    int col, row;
    for(row = 0; row < size; row++)
    {
        for(col = 0; col < size; col++)
        {
            printf("%lf;", grid[(row * size) + col]);
        }
        printf("\n");
    }
    printf("\n\n");
    fflush(stdout);
}

void printSubGrid(double *grid, int size, int rows, int rank)
{
    printf("rank: %d\n", rank);
    int col, row;
    for(row = 0; row < rows; row++)
    {
        for(col = 0; col < size; col++)
        {
            printf("%lf;", grid[(row * size) + col]);
        }
        printf("\n");
    }
    printf("\n\n");
    fflush(stdout);
}

void printSubGridName(char* name, double *grid, int size, int rows, int rank, int time)
{
    printf("name: %s, rank: %d, time: %d\n", name, rank, time);
    int col, row;
    for(row = 0; row < rows; row++)
    {
        for(col = 0; col < size; col++)
        {
            printf("%lf;", grid[(row * size) + col]);
        }
        printf("\n");
    }
    printf("\n\n");
    fflush(stdout);
}