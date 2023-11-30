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

void printGrid(double *grid, int size);
void printSubGrid(double *grid, int size, int rows, int rank);
void printSubGridName(char* name, double *grid, int size, int rows, int rank);
double measureTime(int size, int timeIter);

int main(int argc ,char * argv[]) {
    int size, time, timeIter, number_of_processes, rank = 0;
    int row, col;
    double *gridNew, *gridOld, avg_time;
    struct timeval start, end;
    // Declare our neighbours
    enum DIRECTIONS {DOWN, UP};
    char* neighbours_names[2] = {"down", "up"};
    int neighbours_ranks[2];
    MPI_Status status;
    MPI_Request request;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        
    // if(size != 4)
    // {
    //     printf("This application is meant to be run with 4 processes, not %d.\n", size);
    //     MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    // }

    // Ask MPI to decompose our processes in a 2D cartesian grid for us
    int dims[1] = {0};
    MPI_Dims_create(number_of_processes, 1, dims);

    // Make both dimensions non-periodic
    int periods[1] = {false};

    // Let MPI assign arbitrary ranks if it deems it necessary
    int reorder = true;

    // Create a communicator with a cartesian topology.
    MPI_Comm new_communicator;
    MPI_Cart_create(MPI_COMM_WORLD, 1, dims, periods, reorder, &new_communicator);

    // Let consider dims[1] = Y, so the shift tells us our up and down neighbours
    MPI_Cart_shift(new_communicator, 1, 1, &neighbours_ranks[DOWN], &neighbours_ranks[UP]);

    // Get my rank in the new communicator
    MPI_Comm_rank(new_communicator, &rank);
    
    if(rank == 0)
    {
        // get args
        if (argc > 2) {
            size = atoi(argv[1]);  // Parse size from command line argument
            timeIter = atoi(argv[2]);
        } else {
            size = N;
            timeIter = TIME;
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

        printGrid(gridOld,size);

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
    // int neededRowsPerProcess[number_of_processes] = {0};
    // for(int j = 0; j < number_of_processes; j++)
    // {
    //     if(j == 0 || j == number_of_processes - 1)
    //     {
    //         neededRowsPerProcess[j] = neededRowsPerProcess + 1;
    //     }
    //     else
    //     {
    //         neededRowsPerProcess[j] = neededRowsPerProcess + 2;
    //     }
    // }
    double *subGridOld = (double *)malloc(rowsPerProcess * size * sizeof(double));
    double *subGridNew = (double *)malloc(rowsPerProcess * size * sizeof(double));
    
    double *subGridUpOld = (double *)malloc(size * sizeof(double));
    double *subGridDownOld = (double *)malloc(size * sizeof(double));
    double *subGridUpNew = (double *)malloc(size * sizeof(double));
    double *subGridDownNew = (double *)malloc(size * sizeof(double));
    
    MPI_Scatter(gridOld, size * rowsPerProcess, MPI_DOUBLE, subGridOld, size * rowsPerProcess, MPI_DOUBLE, 0, new_communicator);

    printSubGrid(subGridOld, size, rowsPerProcess, rank);


    // MPI_Sendrecv_replace(subGridUp, size, MPI_INT, neighbours_ranks[UP], 0, neighbours_ranks[DOWN], 0, new_communicator, MPI_STATUS_IGNORE);
    
    // MPI_Sendrecv_replace(subGridDown, size, MPI_INT, neighbours_ranks[DOWN], 0, neighbours_ranks[UP], 0, new_communicator, MPI_STATUS_IGNORE);

    for (time = 0; time < timeIter; time++) 
    {
        // printSubGrid(subGridUp, size, 1, rank);
        // printSubGrid(subGridDown, size, 1, rank);
        for (row = 0; row < rowsPerProcess; row++) 
        {
            for (col = OUTLAY; col < size - OUTLAY; col++) 
            {
                double upVal, downVal = 0;
                if((rank == 0 && row == 0) || (rank == number_of_processes-1 && row == rowsPerProcess-1))
                {
                    subGridNew[row * size + col] = subGridOld[row * size + col];
                    printf("rank: %d, time: %d, Grid: %lf\n", rank, time, subGridNew[row * size + col]);fflush(stdout);
                    continue;
                }
                else if (row == 0 || row == rowsPerProcess-1)
                {
                    upVal = subGridUpNew[col];
                    downVal = subGridDownNew[col];
                }
                else
                {
                    upVal = subGridOld[((row + 1) * size) + col + 1];
                    downVal = subGridOld[((row - 1) * size) + col + 1];
                }
                subGridNew[row * size + col] = subGridOld[row * size + col] +
                                             PHI * ((-4.0) * subGridOld[row * size + col] +
                                                    upVal +
                                                    downVal +
                                                    subGridOld[(row * size) + col + 1] +
                                                    subGridOld[(row * size) + col - 1]);
        
                printf("rank: %d, time: %d, Grid: %lf\n", rank, time, subGridNew[row * size + col]);fflush(stdout);
            }
        }
        printSubGridName("res: ",subGridNew, size, rowsPerProcess, rank);fflush(stdout);
        
        memcpy(subGridUpOld, subGridNew, size*sizeof(double));
        printSubGridName("upold",subGridUpOld, size, 1, rank);fflush(stdout);
        memcpy(subGridDownOld, &subGridNew[(rowsPerProcess-1)*size], size*sizeof(double));
        printSubGridName("downold", subGridDownOld, size, 1, rank);fflush(stdout);

        if(rank == 0)
        { 
            MPI_Isend(subGridDownOld, sizeof(size)/sizeof(double), MPI_DOUBLE, neighbours_ranks[DOWN], 0, new_communicator, &request);
            MPI_Recv(subGridDownNew, sizeof(size)/sizeof(double), MPI_DOUBLE, neighbours_ranks[DOWN], 0, new_communicator, &status);

            printSubGridName("downNew",subGridDownNew, size, 1, rank);fflush(stdout);
           
        }
        else if (rank == number_of_processes-1)
        {
            MPI_Isend(subGridUpOld, sizeof(size)/sizeof(double), MPI_DOUBLE, neighbours_ranks[UP], 0, new_communicator, &request);
            MPI_Recv(subGridUpNew, sizeof(size)/sizeof(double), MPI_DOUBLE, neighbours_ranks[UP], 0, new_communicator, &status);
            printSubGridName("UpNew",subGridUpNew, size, 1, rank);fflush(stdout);
        }
        else
        {
            MPI_Isend(subGridDownOld, sizeof(size)/sizeof(double), MPI_DOUBLE, neighbours_ranks[DOWN], 0, new_communicator, &request);
            
            MPI_Recv(subGridUpNew, sizeof(size)/sizeof(double), MPI_DOUBLE, neighbours_ranks[UP], 0, new_communicator, &status);
            printSubGridName("UpNew",subGridUpNew, size, 1, rank);fflush(stdout);
            
            MPI_Isend(subGridUpOld, sizeof(size)/sizeof(double), MPI_DOUBLE, neighbours_ranks[UP], 0, new_communicator, &request);

            MPI_Recv(subGridDownNew, sizeof(size)/sizeof(double), MPI_DOUBLE, neighbours_ranks[DOWN], 0, new_communicator, &status);
            printSubGridName("UpNew",subGridUpNew, size, 1, rank);fflush(stdout);

        }
        MPI_Barrier(new_communicator);
        

        // MPI_Sendrecv_replace(subGridUp, size, MPI_INT, neighbours_ranks[DOWN], 0, neighbours_ranks[UP], 0, new_communicator, &status);
        // printf("status: %d", status.MPI_ERROR);
        // MPI_Sendrecv_replace(subGridDown, size, MPI_INT, neighbours_ranks[DOWN], 0, neighbours_ranks[UP], 0, new_communicator, &status);
        // printf("status: %d", status.MPI_ERROR);
        // printf(status);
        // Swap grids
        subGridOld = subGridNew;
    }

    MPI_Barrier(new_communicator);
    printf("gather");fflush(stdout);
    MPI_Gather(subGridOld, size * rowsPerProcess, MPI_DOUBLE, gridNew, size * size, MPI_DOUBLE, 0, new_communicator);

    MPI_Barrier(new_communicator);

    if(rank == 0)
    {
        printSubGridName("finalres: ", gridNew, size, rowsPerProcess, rank);fflush(stdout);
    }

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
    // MPI_Finalize();
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
}

void printSubGridName(char* name, double *grid, int size, int rows, int rank)
{
    printf("name: %s, rank: %d\n", name, rank);
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
}