#include <iostream>
#include <mpi.h>
#include <stdlib.h>
#include <chrono>
#include <math.h>
#include <limits>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif


int main (int argc, char* argv[]) {

  if (argc < 2) {
    std::cerr<<"usage: mpirun "<<argv[0]<<" <rows> "<<std::endl;
    return -1;
  }
	int p, n, 
	int **ranks, *leadersArr, *flag, **matrix;
	double  *vect, *reduceVect, *vectorParts, *y,*newVect;
	int rowRank, colRank, rowSize, colSize, matSize, rankCtr, leaderCtr, leaderSize, leaderRank;
	int color, rank, size;
  
	n = atoi(argv[1]);
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
 
  	matSize = n/sqrt(size);
  	matrix = (int **) malloc(matSize*sizeof(int*));
	int dimension = sqrt(size);

	MPI_Comm rowComm;
	MPI_Comm colComm;
	int rowColor = rank/dimension;
	int colColor = rank%dimension;

	MPI_Comm_split(MPI_COMM_WORLD, rowColor, rank, &rowComm);
	MPI_Comm_split(MPI_COMM_WORLD, colColor, rank, &colComm);

	MPI_Comm_rank(rowComm, &rowRank);
	MPI_Comm_size(rowComm, &rowSize);
	MPI_Comm_rank(colComm, &colRank);
	MPI_Comm_size(colComm, &colSize);
	
	for(int i=0;i<matSize;i++){
		matrix[i] = (int *) malloc(matSize*sizeof(int));
	}

	for(int i=0;i<matSize;i++){
		for(int j=0;j<matSize;j++){
			matrix[i][j] = rand()%10 + 1;
		}
	}
	vect = (double *) malloc(n*sizeof(double));
	if(rank == 0){
		for(int i=0;i<n;i++){
			vect[i] = rand()%10 + 1;
		}
	}
	int iter = 0;
	chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();
	while(iter<20){
		MPI_Bcast(vect, n, MPI_DOUBLE, 0, colComm);
		vectorParts = (double *) malloc(matSize*sizeof(double));
		if(rowRank == 0){			
			int ctr = 0;
			reduceVect = (double *) malloc(matSize*sizeof(double));
			for(int i=matSize; i<n; i++){
				vectorParts[ctr++] = vect[i];
				if(ctr % matSize == 0){
					ctr=0;
					MPI_Send(vectorParts, matSize, MPI_DOUBLE, i/matSize, 0, rowComm);
				}
			}
			for(int i=0;i<matSize;i++)
				vectorParts[i] = vect[i];
		}
		else{
			MPI_Recv(vectorParts, matSize, MPI_DOUBLE, 0, 0, rowComm, MPI_STATUS_IGNORE);
		}
		y = (double *) malloc(matSize*sizeof(double));
		for(int i = 0; i<matSize;i++) 
		{
				y[i] = 0;
				for(int j = 0;j<matSize;j++)
					y[i] += matrix[i][j]*vectorParts[j];
		}
		MPI_Reduce(y, reduceVect, matSize, MPI_DOUBLE, MPI_SUM, 0, rowComm);
		if(rowRank == 0){
			if(rank == 0){
				MPI_Status status;
				int rootP = 0;
				newVect = (double *) malloc(n*sizeof(double));
				for(int i=0;i<matSize;i++)
					newVect[i] = reduceVect[i];
				while(rootP<dimension-1){
					MPI_Recv(reduceVect, matSize, MPI_DOUBLE,MPI_ANY_SOURCE,0,colComm,&status);
					int currColRank = status.MPI_SOURCE;
					int offset = currColRank*matSize;
					for(int i=0;i<matSize;i++)
						newVect[i+offset] = reduceVect[i];
					rootP++;
				}	
				for(int i=0;i<n;i++)
					vect[i] = newVect[i];
			}
			else{
				MPI_Send(reduceVect, matSize, MPI_DOUBLE, 0, 0, colComm);
			}
		}
		iter++;
	}
	if(rank==0){
		for(int i=0;i<n;i++)
			cout<<newVect[i]<<"\t";
		cout<<endl;
        chrono::time_point<chrono::system_clock> end = chrono::system_clock::now();
        chrono::duration<double> elapsed_seconds = end-start; 
        cerr<<endl<<elapsed_seconds.count()<<endl;
  }
  	delete[] y;
	delete[] vect;
	delete[] vectorParts;
	MPI_Comm_free(&rowComm);
	MPI_Comm_free(&colComm);
 	MPI_Finalize();
  
  return 0;
}
