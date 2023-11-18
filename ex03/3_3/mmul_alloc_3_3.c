#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>

#define N 5

double sumMatrix(double* a, int size);
void init0Matrix(double* a, int size);
void initRandMatrix(double* a, double* b, double* b_trans, int size);
void initMatrix(double* a, double* b, double* b_trans, int size);
void matrixMultiply(double* a, double* b , double* c , int size);
void matrixMultiplyTranspose(double* a , double* b , double* c, int size);
void matrixMultiplyTransposeDist(double* a, double* b_trans, double* c, int datarowsPerThread, int size);
void printOut(char* name, double* a, int size);
void printOutSub(char* name, double* a, int size, int datarows);

int main(int argc ,char * argv[])
{   
    int rank, number_of_processes;
    int size = 0;  // Set default size to 0
    double cpu_time_used = 0;
    double *A, *B, *B_trans, *C, *a_sub, *b_trans_sub, *c_sub;
    clock_t start, end = 0;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == 0)
    {
        // get args
        if (argc > 1) {
            size = atoi(argv[1]);  // Parse size from command line argument
        } else {
        
            size = N;
        }

        int i;
        for(i = 1; i < number_of_processes; i++)
        {
            MPI_Send(&size, sizeof(size)/sizeof(int), MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    else
    {
        MPI_Recv(&size, sizeof(size)/sizeof(int), MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    printf("size: %d\n", size);
   
    int datarowsPerThread = (int)(size / number_of_processes);
    printf("datarowsPerThread: %d\n", datarowsPerThread);
    if(rank == 0)
    {
        // setup matrices
        A = (double*)malloc(size * size * sizeof(double));
        B_trans = (double*)malloc(size * size * sizeof(double));
        B = (double*)malloc(size * size * sizeof(double));
        C = (double*)malloc(size * size * sizeof(double));
        a_sub = (double*)malloc(size * datarowsPerThread * sizeof(double));
        b_trans_sub = (double*)malloc(size * size * sizeof(double));
        c_sub = (double*)malloc(datarowsPerThread * size * sizeof(double));
        initMatrix(A, B, B_trans, size);
        init0Matrix(C, size);
        printOut("Btrans", B_trans, size);
    }
    else
    {
        a_sub = (double*)malloc(size * datarowsPerThread * sizeof(double)+1);
        b_trans_sub = (double*)malloc(size * size * sizeof(double)+1);
        c_sub = (double*)malloc(size * datarowsPerThread * sizeof(double)+1);
    }
       
    MPI_Scatter(A, size * datarowsPerThread, MPI_DOUBLE, a_sub, size * datarowsPerThread, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Scatter(B_trans, size * datarowsPerThread, MPI_DOUBLE, b_trans_sub, size * datarowsPerThread, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    MPI_Barrier(MPI_COMM_WORLD);
    // printf("B: %f, %d", B_trans, size*datarowsPerThread);
    
    matrixMultiplyTransposeDist(a_sub , b_trans_sub , c_sub, datarowsPerThread, size);

    MPI_Gather(c_sub, size * datarowsPerThread, MPI_DOUBLE, C, size * datarowsPerThread, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if(rank == 0)
    {
        // printOut("C",C,size);
        printOutSub("B", B_trans, size, datarowsPerThread);
        printOutSub("A", A, size, datarowsPerThread);
        printOutSub("C", C, size, datarowsPerThread);


        free(A);
        free(B);
        free(B_trans);
        free(C);
        free(a_sub);
        free(b_trans_sub);
        free(c_sub);
    }
    else
    {

        // printOut("a", a_sub, size*datarowsPerThread);
        printOutSub("c", c_sub, size, datarowsPerThread);
        free(a_sub);
        free(b_trans_sub);
        free(c_sub);
    }

   
    
    // MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    
}

void init0Matrix(double* a, int size)
{
    int row, col;
    for(row = 0; row < size; row++)
    {
        for(col = 0; col < size; col++)
        {
            a[(row * size) + col] = 0;
            // printf("i: %d", (row * col) + col);
        }
    }
}

void initRandMatrix(double* a, double* b, double* b_trans, int size)
{
    int col, row;
    for(row = 0; row < size; row++)
    {
        for(col = 0; col < size; col++)
        {
            // Generate a random double between 0.0 and 1.0
            a[(row * size) + col] = (double)rand() / RAND_MAX;
            b[(row * size) + col] = (double)rand() / RAND_MAX;
            b_trans[(size * col) + row] = b[(row * size) + col];
            
        }
    }
}

void initMatrix(double* a, double* b, double* b_trans, int size)
{
    int col, row;
    for(row = 0; row < size; row++)
    {
        for(col = 0; col < size; col++)
        {
            a[(row * size) + col] = row + col;
            b[(row * size) + col] = row * col;

            b_trans[(size * col) + row] = b[(row * size) + col];
            printf("%f ", a[(row * size) + col]);
        }
        printf("\n");
    }
}

void matrixMultiply(double* a, double* b , double* c , int size)
{
    int row, col, k;
    for(row = 0; row < size; row++)
    {
        for(col = 0; col < size; col++)
        {
            for(k = 0; k < size; k++)
            {
                // Generate a random double between 0.0 and 1.0
                // printf("index: %d a: %f, index: %d b: %f\n",a[(size * row) + k], b[(size * k) + col], (size * k) + col, (size * k));

                c[(row * size) + col] += a[(row * size) + k] * b[(size * k) + col];
            }
            // printf("next element\n");
        }
    }
}

void matrixMultiplyTranspose(double* a , double* b , double* c, int size)
{
    int row, col, k;
    for(row = 0; row < size; row++)
    {
        for(col = 0; col < size; col++)
        {
            for(k = 0; k < size; k++)
            {
                // printf("index: %d a: %f, index: %d b: %f\n",a[(size*row)+k], b[(size*row)+k], (size*row)+k, (size*row)+k);
                c[(row * size) + col] += a[(size * row ) + k] * b[(size * col ) + k];
            }
        }
    }
}

double sumMatrix(double* a, int size)
{
    double sum = 0;
    int row, col = 0;
    for(row = 0; row < size; row++)
    {
        for(col = 0; col < size; col++)
        {
            sum += a[(row * size) + col];
        }
    }
    return sum;
}

void printOut(char* name, double* a, int size)
{
    printf("\n%c\n", *name);
    int row, col;
    for(row = 0 ; row < size ; row++)
    {
        for(col = 0 ; col < size ; col++)
        {
            printf("%f ",a[(row * size) + col]);
        }
        printf("\n");
    }
}

void printOutSub(char* name, double* a, int size, int datarows)
{
    printf("\n%c\n", *name);
    int row, col;
    for(row = 0 ; row < datarows ; row++)
    {
        for(col = 0; col < size; col++)
        {
            printf("%f ",a[(row * size) + col]);
        }
        printf("\n");
    }
}

void matrixMultiplyTransposeDist(double* a, double* b_trans, double* c, int datarowsPerThread, int size)
{
    int row, col, k;
    for(row = 0;  row < datarowsPerThread; row++)
    {
        for(col = 0; col < size; col++)
        {
            for(k = 0; k < size; k++)
            {
                printf("a: %f index: %d, b: %f index2: %d\n",a[(size*row)+k], b_trans[(size*row)+k], (size*row)+k, (size*row)+k);
                c[(row * size) + col] += a[(size * row ) + k] * b_trans[(size * col ) + k];
            }
        }
    }
}

// void transposeMatrix(double a , int size)
// {
//     double buffer ;
//     for(int i = 0; i < size; i++)
//     {
//         for(int j = 0; j < size; j++)
//         {
//             // Generate a random double between 0.0 and 1.0
//             buffer[j][i] = a[i][j];
//         }
//     }
// }