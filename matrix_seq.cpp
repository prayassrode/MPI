#include <iostream>
#include <chrono>
#include <mpi.h>
#include <math.h>
#include <stdio.h>

int main (int argc, char* argv[]) {
 
  int N = atoi(argv[1]);
  int v = 0;
  int ** X  = new int*[N];
  for (int i = 0; i < N ; ++i){		
	X[i] = new int[1];  
  }    
  int **ap  = new int*[N];
  for (int i = 0; i < N ; ++i){		
	ap[i] = new int[N];  
  }
  int ** c  = new int*[N];
  for (int i = 0; i < N ; ++i){		
	c[i] = new int[1];  
  } 
  for (int i=0; i<N; i++) 
  	X[i][0]= 0;
  
    for (int i=0; i<N; i++) 
	{	
	  
      for (int j=0; j<N; j++) 
        ap[i][j]= v++;
    }
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	for(int iter = 0 ; iter<20; iter++)
	{	
	  for (int i=0; i<N; i++) {
		c[i][0] = 0.0;
		for (int j=0; j<N; j++){
		  c[i][0] = c[i][0] + ap[i][j] * X[j][0];
		}
	  }

		std::cout<<"iteration "<<iter<<" is over"<<std::endl;
		for(int i = 0; i < N; ++ i)
		  memcpy(&(X[i][0]), &(c[i][0]), 1 * sizeof(int));
	}
	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	std::cerr<<elapsed_seconds.count()<<std::endl;		  
  }