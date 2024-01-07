#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

#define DELTA_T 5
#define NBODY_DEFAULT 4 
#define TIME_DEFAULT 100
#define GAMMA -6.673*1e-11
#define RAND_TH(a) RAND_MAX * a
// #define DEBUG

//Struct for double3
typedef struct
{
    double x, y, z;
}double3;
//Struct for double4
typedef struct
{
    double x, y, z, w;
}double4;

//Struct Definition for Bodies
typedef struct
{
    int id;
    double4 posMass; /* x = x */
                    /* y = y */
                    /* z = z */
                    /* w = Mass */
    double3 velocity;
    /* x = v_x */
    /* y = v_y */
    /* z = v_z */

    double3 force;

} Body_t;

void showData(Body_t* Bodys, int size);
void showDataLog(char* string, Body_t* Bodys, int size, int rank);
void showAllData(Body_t* Body, int size, int rank);

int main(int argc ,char * argv[]) {
    int number_of_processes, rank, nBody, time, r, i, j;
    int timeIter = 0;
    MPI_Status status;
    MPI_Request request;
    Body_t *Body = NULL;
    Body_t *subBody = NULL;
    struct timeval start, end;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Barrier(MPI_COMM_WORLD);
    
    // setup MPI specific datatypes for custom struct
    MPI_Datatype MPI_Double3;
    MPI_Type_contiguous(3, MPI_DOUBLE, &MPI_Double3);
    MPI_Type_commit(&MPI_Double3);

    MPI_Datatype MPI_Double4;
    MPI_Type_contiguous(4, MPI_DOUBLE, &MPI_Double4);
    MPI_Type_commit(&MPI_Double4);

    MPI_Datatype MPI_Body_t;
    int blocklengths[4] = {1, 1, 1, 1};
    MPI_Aint offsets[4];
    offsets[0] = offsetof(Body_t, id);
    offsets[1] = offsetof(Body_t, posMass);
    offsets[2] = offsetof(Body_t, velocity);
    offsets[3] = offsetof(Body_t, force);

    MPI_Datatype types[4] = {MPI_INT, MPI_Double4, MPI_Double3, MPI_Double3};
    MPI_Type_create_struct(4, blocklengths, offsets, types, &MPI_Body_t);
    MPI_Type_commit(&MPI_Body_t);

    //Synchronisation
    MPI_Barrier(MPI_COMM_WORLD);

    if(rank == 0)
    {
        // get args
        if (argc > 1) 
        {
            // Parse size from command line argument
            nBody = atoi(argv[1]);  
            timeIter = atoi(argv[2]);
        } 
        else 
        {
            nBody = NBODY_DEFAULT;
            timeIter = TIME_DEFAULT;
        }
    }
    //Broadcast number of Bodys and number of iterations
    MPI_Bcast(&nBody, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&timeIter, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //allocate Bodys 
    Body = (Body_t*)malloc(nBody * sizeof(Body_t));

    if(rank == 0)
    {
        // initialisation of Bodys
        int i;
        for(i = 0; i < nBody; i++)
        {
            Body[i].id = i;
            Body[i].posMass.w = (double)rand() / RAND_TH(250000000);
            Body[i].posMass.x = (double)rand() / RAND_TH(100);
            Body[i].posMass.y = (double)rand() / RAND_TH(100);
            Body[i].posMass.z = (double)rand() / RAND_TH(100);
            Body[i].force.x = 0;
            Body[i].force.y = 0;
            Body[i].force.z = 0;
            Body[i].velocity.x = 0;
            Body[i].velocity.y = 0;
            Body[i].velocity.z = 0;
        }
    }

#ifdef DEBUG
    showData(Body, nBody);
#endif

    int bodysPerThread = (int)(nBody / number_of_processes);
    if(bodysPerThread == 0)
    {
        printf("more processes than bodys");fflush(stdout);
        return -1;
    }
    
    //Broadcast Body Data to all ranks
    MPI_Bcast(Body, nBody, MPI_Body_t, 0, MPI_COMM_WORLD);

#ifdef DEBUG
    showAllData(Body, bodysPerThread, rank);
#endif
    
    //get starttime 
    if(rank == 0)
    {
        gettimeofday(&start, NULL);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // loop for number of iterations
    for(time = 0; time < timeIter; time++)
    {
        
        // compute internal forces
        for(j = rank*bodysPerThread; j < (rank+1)*bodysPerThread; j++)
        {
            //set force to zero from last iteration
            Body[j].force.x = 0;
            Body[j].force.y = 0;
            Body[j].force.z = 0;

            for(i = 0; i < nBody; i++)
            {
                if(Body[j].id == Body[i].id)
                {
                    //jump to next element if Body id are the same
                    continue;
                }
                //calculate distance between Bodys
                double rx = Body[j].posMass.x - Body[i].posMass.x;
                double ry = Body[j].posMass.y - Body[i].posMass.y;
                double rz = Body[j].posMass.z - Body[i].posMass.z;
                double rdist_sq = rx * rx + ry * ry + rz * rz;
                double rdist = sqrt(rdist_sq);
                //normalize direction angle 
                rx = rx / rdist;
                ry = ry / rdist;
                rz = rz / rdist;
                //calculate forces
                Body[j].force.x += ((GAMMA * Body[i].posMass.w * Body[j].posMass.w * rx) / rdist_sq);
                Body[j].force.y += ((GAMMA * Body[i].posMass.w * Body[j].posMass.w * ry) / rdist_sq);
                Body[j].force.z += ((GAMMA * Body[i].posMass.w * Body[j].posMass.w * rz) / rdist_sq);
            }

            //calculate velocity for final object
            Body[j].velocity.x = (Body[j].force.x  / Body[j].posMass.w) * DELTA_T;
            Body[j].velocity.y = (Body[j].force.y  / Body[j].posMass.w) * DELTA_T;
            Body[j].velocity.z = (Body[j].force.z  / Body[j].posMass.w) * DELTA_T;
        
            Body[j].posMass.x += Body[j].velocity.x * DELTA_T;
            Body[j].posMass.y += Body[j].velocity.y * DELTA_T;
            Body[j].posMass.z += Body[j].velocity.z * DELTA_T;

        }
        subBody = (Body_t*)malloc(bodysPerThread * sizeof(Body_t));
        memcpy(subBody, &Body[rank*bodysPerThread], bodysPerThread * sizeof(Body_t));

        //get all updated Bodys from other tasks 
        MPI_Allgather(subBody, bodysPerThread, MPI_Body_t, Body, bodysPerThread, MPI_Body_t, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
    }
    //collect final data
    MPI_Gather(&Body[rank*bodysPerThread], bodysPerThread, MPI_Body_t, Body, bodysPerThread, MPI_Body_t, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    if(rank == 0)
    {
        gettimeofday(&end, NULL);
        double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
        printf("Nbody,%d,numberofProcesses,%d,time,%lfs\n", nBody,number_of_processes,elapsed_time);fflush(stdout);
        // showData(Body, nBody);
    }

    MPI_Finalize();
    free(Body);
}

void showData(Body_t* Bodys, int size)
{
    int a;
    for(a = 0; a < size; a++)
    {
        printf("%d;%.1lf;%.1lf;%.1lf;", Bodys[a].id, Bodys[a].posMass.x, Bodys[a].posMass.y, Bodys[a].posMass.z);
    }
    printf("\n");
}

void showAllData(Body_t* Body, int size, int rank)
{
    int a;
    for(a = 0; a < rank*size; a++)
    {
        printf("id %d pos_x %.1f pos_y %.1f pos_z %.1f v_x %.1f v_y %.1f v_z %.1f m %.1f F_x %.1f F_y %.1f F_z %.1f\n", 
        Body[a].id, Body[a].posMass.x, Body[a].posMass.y, Body[a].posMass.z,
        Body[a].velocity.x, Body[a].velocity.y, Body[a].velocity.z,Body[a].posMass.w,
        Body[a].force.x, Body[a].force.y, Body[a].force.z);
    }
    printf("\n\n");
}