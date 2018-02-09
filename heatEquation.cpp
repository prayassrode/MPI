#include <iostream>
#include <mpi.h>
#include <stdlib.h>
#include <cstdlib>
#include <chrono>
#include <math.h>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

double **H, **Hp;
int n, p, powers;
MPI_Request reqSendTop, reqSendBottom, reqRecTop, reqRecBottom;
MPI_Status *statusTop, *statusBottom;
double *recTop, *recBottom;

void fillRand(){
	for(int i=0; i<n/p; i++){
		for(int j=0; j<n; j++){
			H[i][j] = (rand()%1000)*1.0;
		}
	}
}
void printMatrix(){
	for(int i=0; i<n/p; i++){
		for(int j=0; j<n; j++){
			cout<<H[i][j]<<"\t";
		}
		cout<<endl;
	}
}

void communicate(int rank){
	if(rank==0){
		recBottom = new double[n];
		MPI_Isend(H[n/p-1], n, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &reqSendBottom);reqRecTop(recBottom, n, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &reqRecBottom);
		calculate();
		MPI_Wait(&reqRecBottom, statusBottom);
		calculate(rank, recBottom);
	}
	else if(rank==p-1){
		recTop = new double[n];

		MPI_Isend(H[0], n, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &reqSendTop);
		MPI_Irecv(recTop, n, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &req_recTop);
		calculate();
		MPI_Wait(&req_recTop, statusTop);
		calculate(rank, recTop);
	}
	else{
		recTop = new double[n];
		recBottom = new double[n];
		MPI_Isend(H[n/p-1], n, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &reqSendBottom);
		reqRecTop(H[0], n, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &reqSendTop);
		MPI_Irecv(recBottom, n, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &reqRecBottom);
		MPI_Irecv(recTop, n, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &req_recTop);
		calculate();
		MPI_Wait(&req_recTop, statusTop);
		MPI_Wait(&reqRecBottom, statusBottom);
		calculate(rank, recTop, recBottom); 
	}
}
void calculate(){
  for(int i=1; i<n/p-1; i++){
	for(int j=0; j<n; j++){
		if(j==0)
		  Hp[i][j]=(H[i-1][j]+H[i-1][j+1]+H[i][j+1]+H[i+1][j+1]+H[i+1][j]+H[i][j])/6;
		else if(j==n-1)
		  Hp[i][j]=(H[i-1][j-1]+H[i-1][j]+H[i+1][j]+H[i+1][j-1]+H[i][j-1]+H[i][j])/6;
		else
		  Hp[i][j]=(H[i-1][j-1]+H[i-1][j]+H[i-1][j+1]+H[i][j+1]+H[i+1][j+1]+H[i+1][j]+H[i+1][j-1]+H[i][j-1]+H[i][j])/9;
	}
  }
}
void calculate(int rank, double *recv){
	if(rank==0){
		int i=0;
		for(int j=0; j<n; j++){
			if(j==0)
				Hp[0][j]=(H[i][j+1]+H[i+1][j+1]+H[i+1][j]+H[i][j])/4;
			else if(j==n-1)
				Hp[0][j]=(H[i+1][j]+H[i+1][j-1]+H[i][j-1]+H[i][j])/4;
			else
				Hp[0][j]=(H[i][j+1]+H[i+1][j+1]+H[i+1][j]+H[i+1][j-1]+H[i][j-1]+H[i][j])/6;
		}
		i=n/p-1;
		for(int j=0; j<n; j++){
			if(j==0)
		  		Hp[i][j]=(H[i-1][j]+H[i-1][j+1]+H[i][j+1]+recv[1]+recv[0]+H[i][j])/6;
			else if(j==n-1)
		  		Hp[i][j]=(H[i-1][j-1]+H[i-1][j]+recv[n-1]+recv[n-2]+H[i][j-1]+H[i][j])/6;
			else
		  		Hp[i][j]=(H[i-1][j-1]+H[i-1][j]+H[i-1][j+1]+H[i][j+1]+recv[j+1]+recv[j]+recv[j-1]+H[i][j-1]+H[i][j])/9;
		}
	}
	else if(rank==p-1){
		int i=n/p-1;
		for(int j=0; j<n; j++){
			if(j==0)
				Hp[i][j]=(H[i][j+1]+H[i-1][j+1]+H[i][j]+H[i-1][j])/4;
			else if(j==n-1)
				Hp[i][j]=(H[i][j]+H[i][j-1]+H[i-1][j-1]+H[i-1][j])/4;
			else
		  		Hp[i][j]=(H[i-1][j-1]+H[i-1][j]+H[i-1][j+1]+H[i][j+1]+H[i][j-1]+H[i][j])/6;
		
		}
		i=0;
		for(int j=0; j<n; j++){
			if(j==0)
				Hp[0][j]=(H[i][j+1]+H[i+1][j+1]+H[i+1][j]+H[i][j]+recv[0]+recv[1])/6;
			else if(j==n-1)
				Hp[0][j]=(H[i+1][j]+H[i+1][j-1]+H[i][j-1]+H[i][j]+recv[n-1]+recv[n-2])/6;
			else
				Hp[0][j]=(H[i][j+1]+H[i+1][j+1]+H[i+1][j]+H[i+1][j-1]+H[i][j-1]+H[i][j]+recv[j-1]+recv[j]+recv[j+1])/9;
		
		}
	}
	
	
}
void calculate(int rank, double *recTop, double *recBottom){
	
	int i=n/p-1;
	for(int j=0; j<n; j++){
		if(j==0)
	  		Hp[i][j]=(H[i-1][j]+H[i-1][j+1]+H[i][j+1]+recBottom[1]+recBottom[0]+H[i][j])/6;
		else if(j==n-1)
	  		Hp[i][j]=(H[i-1][j-1]+H[i-1][j]+recBottom[n-1]+recBottom[n-2]+H[i][j-1]+H[i][j])/6;
		else
	  		Hp[i][j]=(H[i-1][j-1]+H[i-1][j]+H[i-1][j+1]+H[i][j+1]+recBottom[j+1]+recBottom[j]+recBottom[j-1]+H[i][j-1]+H[i][j])/9;
	}
	i=0;
	for(int j=0; j<n; j++){
		if(j==0)
			Hp[0][j]=(H[i][j+1]+H[i+1][j+1]+H[i+1][j]+H[i][j]+recTop[0]+recTop[1])/6;
		else if(j==n-1)
			Hp[0][j]=(H[i+1][j]+H[i+1][j-1]+H[i][j-1]+H[i][j]+recTop[n-1]+recTop[n-2])/6;
		else
			Hp[0][j]=(H[i][j+1]+H[i+1][j+1]+H[i+1][j]+H[i+1][j-1]+H[i][j-1]+H[i][j]+recTop[j-1]+recTop[j]+recTop[j+1])/9;
	
	}

}

int main (int argc, char* argv[]) {

  if (argc < 3) {
    std::cerr<<"usage: mpirun "<<argv[0]<<"<Row Size> <Iterations>"<<std::endl;
    return -1;
  }
  
  int rank, world_size;
  
  MPI_Init(&argc, &argv);
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
  
  p = world_size;
  n = atoi(argv[1]);
  powers = atoi(argv[2]);
  H = new double*[n/world_size];
  Hp = new double*[n/world_size];
   
  for(int i=0; i<n/world_size; i++){
	H[i] = new double[n];
  	Hp[i] = new double[n];
  }
  srand(rank);
  fillRand();
  

  chrono::time_point<chrono::system_clock> start;
  if(rank==0)
	start = chrono::system_clock::now();
  for(int k=0; k<powers; k++){
  	communicate(rank);
	MPI_Barrier(MPI_COMM_WORLD);
	for(int i=0; i<n/p; i++)
		for(int j=0; j<n; j++)
			H[i][j] = Hp[i][j];
  }
  MPI_Barrier(MPI_COMM_WORLD);
  if(rank==0){
 	chrono::time_point<chrono::system_clock> end = chrono::system_clock::now();
  	chrono::duration<double> elapsed_seconds = end-start; 
  	cerr<<endl<<elapsed_seconds.count()<<endl;
  }

  MPI_Finalize();
  for(int i=0; i<n/p; i++){
	 delete[] H[i];
  	 delete[] Hp[i];
  }
  delete[] H, Hp, recTop, recBottom; 
  return 0;
}

