#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define N 2048

void init0Matrix(double a[N][N], int size);
void initRandMatrix(double a[N][N], double a_trans[N][N], double b[N][N], int size);
void matrixMultiply(double a[N][N], double b[N][N], double c[N][N], int size);
void matrixMultiplyTranspose(double a[N][N], double b[N][N], double c[N][N], int size);

int main(int argc, const char **argv) 
{
    clock_t start, end = 0;
    double cpu_time_used = 0;

    // setup matrices

    double A[N][N];
    double A_transposed[N][N];
    double B[N][N];
    double C[N][N];

    srand(time(NULL));   // Initialization, should only be called once.

    initRandMatrix(A, A_transposed, B, N);
    init0Matrix(A, N);

    {
    start = clock();
    matrixMultiply(A, B, C, N);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("CPU time used: %f seconds\n", cpu_time_used);
    }

    {
    start = clock();
    matrixMultiplyTranspose(A_transposed, B, C, N);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("CPU time used: %f seconds\n", cpu_time_used);
    }

}

void init0Matrix(double a[N][N], int size)
{
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            a[i][j] = 0;
        }
    }
}

void initRandMatrix(double a[N][N], double a_trans[N][N], double b[N][N], int size)
{
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            // Generate a random double between 0.0 and 1.0
            a[i][j] = (double)rand() / RAND_MAX;
            a_trans[j][i] = a[i][j];
            b[i][j] = (double)rand() / RAND_MAX;
        }
    }

}

void matrixMultiply(double a[N][N], double b[N][N], double c[N][N], int size)
{
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            for(int k = 0; k < size; k++)
            {
                // Generate a random double between 0.0 and 1.0
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }

}

void matrixMultiplyTranspose(double a[N][N], double b[N][N], double c[N][N], int size)
{
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            for(int k = 0; k < size; k++)
            {
                c[i][j] += a[i][k] * b[j][k];
            }
        }
    }

}

// void transposeMatrix(double a[N][N], int size)
// {
//     double buffer[N][N];

//     for(int i = 0; i < size; i++)
//     {
//         for(int j = 0; j < size; j++)
//         {
//             // Generate a random double between 0.0 and 1.0
//             buffer[j][i] = a[i][j];
//         }
//     }
// }
