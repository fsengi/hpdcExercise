#include <stdlib.h>
#include <stdio.h>

#define N 10
#define OUTLAY 1
#define TIME 10
#define PHI 0.24

void printGrid(double *grid, int size);

int main(int argc ,char * argv[])
{
    int size, time, timeIter = 0;
    int row, col;
    double *gridNew, *gridOld;
    // get args
    if (argc > 2) {
        size = atoi(argv[1]);  // Parse size from command line argument
        timeIter = atoi(argv[2]);
    } else {
        size = N;
        timeIter = TIME;
    }

    // allocate grid
    gridOld = (double*)malloc(size * size * sizeof(double));
    gridNew = (double*)malloc(size * size * sizeof(double));

    // initialize
    for(col = 0; col < size; col++)
    {
        if (col >= size/4 && col <= 3*size/4)
        {
            gridOld[1 * size + col] = 127;
        }        
    }
    printGrid(gridOld, size);
    
    for(time = OUTLAY; time < timeIter; time++)
    {
        for(row = OUTLAY; row < size-OUTLAY; row++)
        {
            for(col = 0; col < size-OUTLAY; col++)
            {
                gridNew[row * size + col] = gridOld[row * size + col] + PHI * ((-4)*gridOld[row * size + col] + 
                                                                                    gridOld[((row + 1) * size) + col] + 
                                                                                    gridOld[((row - 1) * size) + col] + 
                                                                                    gridOld[(row * size) + col + 1] +
                                                                                    gridOld[(row * size) + col - 1]);

                if(gridNew[row * size + col] > 127)
                {
                    gridNew[row * size + col] = 127;
                }
                else if (gridNew[row * size + col] < 0)
                {
                    gridNew[row * size + col] = 0;
                }
            }
        }
        gridOld = gridNew;
        printGrid(gridNew, size);
    }
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
