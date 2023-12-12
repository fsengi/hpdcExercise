#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <stddef.h>

#define NBODY_DEFAULT 4 
#define TIME_DEFAULT 2

// typedef struct {
//     double x;
//     double y;
//     double z;
// }coordinate;

typedef struct {
    double mass;
    double pos_x;
    double pos_y;
    double pos_z;
    double v_x;
    double v_y;
    double v_z;
    double a_x;
    double a_y;
    double a_z;
}BodyProp;


int main(int argc ,char * argv[]) {
    int number_of_processes, rank, nBody, timeIter, bodysPerThread;
    MPI_Status status;
    MPI_Request request;
    BodyProp *Body = NULL;
    BodyProp *subBody = NULL;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    
    /* create a type for struct car */
    const int nitems=2;
    int          blocklengths[10] = {1,1,1,1,1,1,1,1,1,1};
    MPI_Datatype types[2] = {   MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, 
                                MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    MPI_Datatype mpi_BodyProp;
    MPI_Aint     offsets[10];

    offsets[0] = offsetof(BodyProp, mass);
    offsets[1] = offsetof(BodyProp, pos_x);
    offsets[2] = offsetof(BodyProp, pos_y);
    offsets[3] = offsetof(BodyProp, pos_z);
    offsets[4] = offsetof(BodyProp, v_x);
    offsets[5] = offsetof(BodyProp, v_y);
    offsets[6] = offsetof(BodyProp, v_z);
    offsets[7] = offsetof(BodyProp, a_x);
    offsets[8] = offsetof(BodyProp, a_y);
    offsets[9] = offsetof(BodyProp, a_z);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_BodyProp);
    MPI_Type_commit(&mpi_BodyProp);

    
    if(rank == 0)
    {
        // get args
        if (argc > 1) 
        {
            nBody = atoi(argv[1]);  // Parse size from command line argument
            timeIter = atoi(argv[2]);
        } 
        else 
        {
            nBody = NBODY_DEFAULT;
            timeIter = TIME_DEFAULT;
        }
        
        Body = (BodyProp*)malloc(nBody * sizeof(BodyProp));
        int i;
        for(i = 1; i < number_of_processes; i++)
        {
            MPI_Send(&nBody, sizeof(nBody)/sizeof(int), MPI_INT, i, 1, MPI_COMM_WORLD);
            MPI_Send(&timeIter, sizeof(timeIter)/sizeof(int), MPI_INT, i, 2, MPI_COMM_WORLD);
        }

        // initialisation 
        for(i = 0; i < nBody; i++)
        {
            Body[i].pos_x = (double)rand();
            Body[i].pos_y = (double)rand();
            Body[i].pos_z = (double)rand();
            Body[i].mass = (double)rand();
            Body[i].a_x = 0;
            Body[i].a_y = 0;
            Body[i].a_z = 0;
            Body[i].v_x = 0;
            Body[i].v_y = 0;
            Body[i].v_z = 0;
        }
    }
    else
    {
        MPI_Recv(&nBody, sizeof(nBody)/sizeof(int), MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&timeIter, sizeof(timeIter)/sizeof(int), MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status);
    }

    bodysPerThread = nBody / number_of_processes;
    subBody = (BodyProp*)malloc(bodysPerThread * sizeof(BodyProp));
    

    //Scatter data
    printf("size %d\n", sizeof(Body));
    MPI_Scatter(Body, (bodysPerThread * sizeof(Body))/(sizeof(double)*10), subBody, mpi_BodyProp, (bodysPerThread * sizeof(Body))/(sizeof(double)*10) , mpi_BodyProp, 0, MPI_COMM_WORLD);


    printf("Body %lf ", subBody->pos_x);
    // printSubGrid(subGridOld, size, rowsPerProcess, rank);

    MPI_Finalize();

    free(subBody);
    free(Body);
}