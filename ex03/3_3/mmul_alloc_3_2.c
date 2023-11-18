#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>

#define N 2048
#define SECTION3_2

double sumMatrix(double* a, int size);
void init0Matrix(double* a, int size);
void initRandMatrix(double* a, double* a_trans, double* b, int size);
void initMatrix(double* a, double* a_trans, double* b, int size);
void matrixMultiply(double* a, double* b , double* c , int size);
void matrixMultiplyTranspose(double* a , double* b , double* c, int size);
void printOut(char* name, double* a, int size);

int main(int argc ,char * argv[])
{   

      // get args
    size_t size = 0;  // Set default size to 0
    if (argc > 1) {
        size = atoi(argv[1]);  // Parse size from command line argument
    } else {
       
        size = N;
    }
    int rank, number_of_processes;
    clock_t start, end = 0;
    double cpu_time_used = 0;
    MPI_Status status;
    double *A, *B, *B_transposed, *C, *a_sub, *b_transposed_sub, *c_sub;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // printf("hello from process %d of %d\n", rank, number_of_processes);
    int datarowsPerThread = (int)(size / number_of_processes) + 1;
    MPI_Barrier(MPI_COMM_WORLD);
    
    if(rank == 0)
    {
        // setup matrices
        A = (double*)malloc(size * size * sizeof(double));
        B_transposed = (double*)malloc(size * size * sizeof(double));
        B = (double*)malloc(size * size * sizeof(double));
        C = (double*)malloc(size * size * sizeof(double));
    }else
    {
        a_sub = (double*)malloc(size * datarowsPerThread * sizeof(double));
        b_transposed_sub = (double*)malloc(size * size * sizeof(double));
        c_sub = (double*)malloc(size * datarowsPerThread * sizeof(double));
    }

    if(rank == 0)
    {

        srand(time(NULL));   // Initialization, should only be called once.
        initRandMatrix(A, B, B_transposed, size);
        init0Matrix(C, size);

        start = clock();
        matrixMultiply(A, B, C, size);
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        double sum = sumMatrix(C, size);
        printf("execution time: %.2fs, Flops: %f, sum:%f\n", cpu_time_used, (size*size*size*2)/(cpu_time_used*1e9), sum);

        init0Matrix(C, size);

        printf("matrixMultiplyTranspose\n");
        start = clock();
        matrixMultiplyTranspose(A, B_transposed, C, size);
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        sum = sumMatrix(C, size);
        printf("execution time: %.2fs, Flops: %f, sum:%f\n", cpu_time_used, (size*size*size*2)/(cpu_time_used*1e9), sum);
    }
    else{
       
    }
}

void init0Matrix(double* a , int size)
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

void initMatrix(double* a, double* b_trans, double* b, int size)
{
    int col, row;
    for(row = 0; row < size; row++)
    {
        for(col = 0; col < size; col++)
        {
            a[(row * size) + col] = row + col;
            b[(row * size) + col] = row * col;

            b_trans[(size * col) + row] = b[(row * size) + col];
        }
    }
}

void matrixMultiply(double* a , double* b , double* c , int size)
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

void matrixMultiplyTranspose(double* a , double* b , double* c , int size)
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