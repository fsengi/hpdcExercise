#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <omp.h>

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 5120000 / 8
#endif /* ifndef BLOCK_SIZE */

void mmul_unoptimized(const double *A, const double *B, double *C, size_t dim);
void mmul_optimized(const double *A, const double *B, double *C, size_t dim);

int main(int argc, const char **argv) {
    // get args
    size_t size = 0;  // Set default size to 0
    if (argc > 1) {
        size = atoi(argv[1]);  // Parse size from command line argument
    } else {
        fprintf(stderr, "Usage: %s <size>\n", argv[0]);
        return 1;
    }

    // printf("======================\n");
    // // Set the number of threads to, for example, 4
    // omp_set_num_threads(2);
    // unsigned int nr_threads = omp_get_thread_num();
    // printf("Number of threads: %d\n", nr_threads);

    // setup matrices
    double *A = (double *)calloc(size * size, sizeof(double));
    double *B = (double *)calloc(size * size, sizeof(double));
    double *C = (double *)calloc(size * size, sizeof(double));

    // setup rng
    unsigned int seed = 42;  // Set a seed for reproducibility
    #pragma omp parallel
    {
        unsigned int my_seed = seed + omp_get_thread_num();
        size_t i; 
        #pragma omp for
        for (i = 0; i < size * size; ++i) {
            A[i] = rand_r(&my_seed) / (double)RAND_MAX * 100.0;
        }
        #pragma omp for
        for (i = 0; i < size * size; ++i) {
            B[i] = rand_r(&my_seed) / (double)RAND_MAX * 100.0;
        }
    }

    double start_unopt = omp_get_wtime();
    mmul_unoptimized(A, B, C, size);
    double stop_unopt = omp_get_wtime();
    double time_unopt = (stop_unopt - start_unopt) * 1e9;  // Convert to nanoseconds

    double start_opt = omp_get_wtime();
    mmul_optimized(A, B, C, size);
    double stop_opt = omp_get_wtime();
    double time_opt = (stop_opt - start_opt) * 1e9;  // Convert to nanoseconds

    double FLOP = size * size * size * 2.0;
    printf("======================\n");
    printf("Unoptimized :\n");
    printf("Time %.0lf ns\n", time_unopt);
    printf("=> %.2lf GFLOP/s\n", FLOP / time_unopt);
    printf("----------------------\n");
    printf("Optimized :\n");
    printf("Time %.0lf ns\n", time_opt);
    printf("=> %.2lf GFLOP/s\n", FLOP / time_opt);
    printf("======================\n");

    free(A);
    free(B);
    free(C);

    return 0;
}

void mmul_unoptimized(const double *A, const double *B, double *C, size_t dim) {
    /*
     * Naive matrix multiply for square matrices (NxN)
     * A, B: input matrices
     * C: output matrix (has to be initialized to zero)
     * dim: number of rows (or columns in this case) of the matrix
     */
    size_t i; 
    size_t j; 
    size_t k;
    for (i = 0; i < dim; ++i) {
        for (j = 0; j < dim; ++j) {
            for (k = 0; k < dim; ++k) {
                C[i * dim + j] += A[i * dim + k] * B[k * dim + j];
            }
        }
        // printf("Executed one task\n");
    }
}

void mmul_optimized(const double *A, const double *B, double *C, size_t dim) {
    /*
     * Optimized version of matrix multiply for square matrices (NxN)
     * Memory bandwidth is increased by better cache utilization via
     * block-wise access to elements instead of linear traversal
     * A, B : input matrices
     * C: output matrix (has to be initialized to zero)
     * dim: number of rows (or columns in this case) of the matrix
     */

    size_t ii;
    size_t jj;
    size_t kk;

    for (ii = 0; ii < dim; ii += BLOCK_SIZE) {
        for (jj = 0; jj < dim; jj += BLOCK_SIZE) {
            for (kk = 0; kk < dim; kk += BLOCK_SIZE) {
                size_t i, j, k;
#pragma omp parallel for collapse(3)
                for (i = ii; i < ii + BLOCK_SIZE; ++i) {
                    for (j = jj; j < jj + BLOCK_SIZE; ++j) {
                        for (k = kk; k < kk + BLOCK_SIZE; ++k) {
                            if (i < dim && j < dim && k < dim) {
                                C[i * dim + j] += A[i * dim + k] * B[k * dim + j];
                            }
                        }
                    }
                }
            }
        }
    }
}
