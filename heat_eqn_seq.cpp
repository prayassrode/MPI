#include <cstdlib>
#include <chrono>
#include <iostream>
#include <cmath>

using namespace std;

#define MAXNUM 100
int n, p;
double** H;

void randFill()
{
    for(int i = 0;i<n; i++)
    {
        for(int j = 0;j<n;j++)
            H[i][j] = (rand()%MAXNUM + 1)*1.0;
    }
    return;
}

double calValue(int i, int j)
{
    double r = 0.0;
    if(i == 0 || i == n-1 || j == 0 || j == n-1)
    {
        if((i == 0 && j == 0))
            r = 6*H[i][j]+ H[i][j+1]+H[i+1][j] +H[i+1][j+1];
        else if(i == 0 && j == n-1)
            r = 6*H[i][j]+H[i][j-1] +H[i+1][j]+H[i+1][j-1];
        else if(i == n-1 && j == 0)
            r = 6*H[i][j] + H[i][j+1] + H[i-1][j] + H[i-1][j+1];
        else if(i == 0)
            r = H[i][j-1] + 4*H[i][j] + H[i][j+1] + H[i+1][j-1]+ H[i+1][j] + H[i+1][j+1];
        else if(i == n-1)
            r = H[i-1][j-1] + H[i-1][j] +H[i-1][j+1] + H[i][j-1] + 4*H[i][j] + H[i][j+1];
        else if(j == 0)
            r = H[i-1][j] + H[i-1][j+1] +4*H[i][j] + H[i][j+1] + H[i+1][j] + H[i+1][j+1];
        else if(i == n-1 && j == n-1)
            r = 6*H[i][j] + H[i-1][j] + H[i][j-1] +H[i-1][j-1];
        else if(j == n-1)
            r = H[i-1][j-1] + H[i-1][j] + H[i][j-1] + 4*H[i][j] + H[i+1][j-1] + H[i+1][j];
    }
    else
        r = H[i-1][j-1]+H[i-1][j]+H[i-1][j+1]+H[i][j-1]+H[i][j]+H[i][j+1]+H[i+1][j-1]+H[i+1][j]+H[i+1][j+1];
    return r/9.0;
}

void heatEq(){
  double **temp = new double*[n];
  for(int i = 0; i < n; i++)
        temp[i] = new double[n];
    while( p--){
        for(int i = 0; i<n;i++) 
        {
            for(int j = 0;j<n;j++)
                temp[i][j] = calValue(i,j);
        }
        for(int i = 0; i<n;i++) 
        {
            for(int j = 0;j<n;j++)
                H[i][j] = temp[i][j];
        }
    }
    delete[] *temp;
    return; 
}

int main (int argc, char* argv[]) {
    srand(time(NULL));
    if (argc < 3) {
        std::cerr<<"usage: mpirun "<<argv[0]<<" <n> <p>"<<std::endl;
        return -1;
    }  
    n = atoi(argv[1]);
     p = atoi(argv[2]); 
    H=new double*[n];
    for(int i = 0; i < n; i++)
        H[i] = new double[n];
    randFill();
    std::chrono::time_point<std::chrono::system_clock> start_time=std::chrono::system_clock::now(); 
    heatEq();

    std::chrono::time_point<std::chrono::system_clock> end=std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start_time;
    std::cerr<<elapsed_seconds.count()<<std::endl;
    
    return 0;
}
