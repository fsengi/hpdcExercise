#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#define OUTLAY 1
#define PHI 0.24

void printGrid(double *grid, int size);

double measureTime(int size, int timeIter);

int main() {
    int grid_sizes[] = {128, 512, 1024, 2048, 4096};
    int num_sizes = sizeof(grid_sizes) / sizeof(grid_sizes[0]);
    int timeIter = 100;  // Number of iterations

    printf("Grid size\tTime/iteration\tFlops total\tGFLOP/s\n");

    for (int i = 0; i < num_sizes; i++) {
        int size = grid_sizes[i];

        // Calculate average time dynamically
        double avg_time = measureTime(size, timeIter);

        // Calculate FLOPs per iteration
        double flops_per_iteration = 7.0 * size * size;

        // Calculate total FLOPs for the current size
        double total_flops = flops_per_iteration * timeIter;

        // Calculate GFLOP/s
        double gflops = flops_per_iteration / (avg_time * 1e9);

        printf("%dx%d\t\t%.6lf\t\t%.0lf\t\t%.3lf\n", size, size, avg_time, total_flops, gflops);
    }

    return 0;
}

double measureTime(int size, int timeIter) {
    int row, col;
    struct timeval start, end;

    double *gridOld, *gridNew;

    // allocate grid
    gridOld = (double *)malloc(size * size * sizeof(double));
    gridNew = (double *)malloc(size * size * sizeof(double));

    // initialize
    for (col = 0; col < size; col++) {
        if (col >= size / 4 && col <= 3 * size / 4) {
            gridOld[col] = 127;
        }
    }

    gettimeofday(&start, NULL);

    for (int time = 0; time < timeIter; time++) {
        for (row = OUTLAY; row < size - OUTLAY; row++) {
            for (col = OUTLAY; col < size - OUTLAY; col++) {
                gridNew[row * size + col] = gridOld[row * size + col] +
                                             PHI * ((-4.0) * gridOld[row * size + col] +
                                                    gridOld[((row + 1) * size) + col] +
                                                    gridOld[((row - 1) * size) + col] +
                                                    gridOld[(row * size) + col + 1] +
                                                    gridOld[(row * size) + col - 1]);

                if (gridNew[row * size + col] > 127) {
                    gridNew[row * size + col] = 127;
                } else if (gridNew[row * size + col] < 0) {
                    gridNew[row * size + col] = 0;
                }
            }
        }
        // Swap grids
        double *temp = gridOld;
        gridOld = gridNew;
        gridNew = temp;
    }

    gettimeofday(&end, NULL);

    // Free allocated memory
    free(gridOld);
    free(gridNew);

    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

    return elapsed_time / timeIter;
}
