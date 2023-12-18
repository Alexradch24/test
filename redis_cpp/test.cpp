#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <sw/redis++/redis++.h>




using namespace std;
using namespace sw::redis;

int N = 100;

int main(){
    //ofstream fout("Vectors.txt");
    auto redis = Redis("tcp://127.0.0.1:6379/inp");
    int v[N][N];
    string key, value;
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            v[i][j] = rand()%N;
            key = to_string(i) + ' ' + to_string(j);
            value = to_string(v[i][j]);
            redis.set(key,value); 
            //fout << v[i][j] << ' ';
        }
        //fout << '\n';
    }
    //fout.close();
    return 0;
}
