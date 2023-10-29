// A very short MPI test routine to show the involved node names
#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
int main ( int argc, char **argv ) {
    int size,rank;
    char hostname[50];
    MPI_Init ( &argc, &argv );

    MPI_Comm_rank ( MPI_COMM_WORLD, &rank); // Who am I?
    MPI_Comm_size ( MPI_COMM_WORLD, &size ); // How many others?

    gethostname ( hostname, 50 );
    printf ( "Hello Parallel World! (rank %2d/%2d on %s)\n", rank, size, hostname );
    MPI_Finalize ();
    return 0;
}