#define N 128
#define OUTLAY 1
#define TIME 1
#define PHI 0.24

void printGrid(double ** grid, int size);

int main(int argc ,char * argv[])
{
    int size, outergrid, time, timeIter = 0;
    int row, col;
    double **gridNew, **gridOld, **temp;
    // get args
    if (argc > 2) {
        size = atoi(argv[1]);  // Parse size from command line argument
        timeIter = atoi(argv[2]);
    } else {
        size = N;
        timeIter = TIME;
    }

    outergrid = size + 2*OUTLAY;

    // allocate grid
    gridOld = (double**)malloc(size * sizeof(double*));
    gridNew = (double**)malloc(size * sizeof(double*));
    for(row = 0; row < size; row++)
    {
        gridOld[row] = (double*)calloc(outergrid * outergrid * sizeof(double));
        gridNew[row] = (double*)calloc(outergrid * outergrid * sizeof(double));
    }
    // initialize
    for(col = 1; col < size; col++)
    {
        if (col > size/4 && col < 3*size/4)
        {
            gridOld[col][1] = 127;
        }
    }
    printGrid(gridOld, outergrid);
    
    for(time = 0; time < timeIter; time++)
    {
        for(col = OUTLAY; col < size+OUTLAY; col++)
        {
            for(row = OUTLAY; row < size+OUTLAY; row++)
            {
                gridNew[col][row] = gridOld[col][row] + PHI * ((-4)*gridOld[col][row] + gridOld[col+1][row]+ gridOld[col-1][row]+ gridOld[col][row-1]+gridOld[col][row+1]);
                if(gridNew[col][row]>127)
                {
                    gridNew[col][row] = 127;
                }
                else if (gridNew[col][row]>127 < 0)
                {
                    gridNew[col][row] = 0;
                }
            }
        }
        gridOld = gridNew;
        printGrid(gridNew, outergrid);
    }
}

void printGrid(double ** grid, int size)
{
    int col, row;
    for(col = 0; col < size+2*OUTLAY; col++)
    {
        for(row = 0; row < size+2*OUTLAY; row++)
        {
            printf("%d,", grid[col][row]);
        }
        printf("\n");
    }
}
