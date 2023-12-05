#include <iostream>
#include <vector>
#include <fstream>

using namespace std;
int N = 1024;

int main(){
    ofstream fout("Vectors.txt");
    int v[N][N];
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            v[i][j] = rand()%1024;
            fout << v[i][j] << ' ';
        }
        fout << '\n';
    }
    fout.close();
    return 0;
}
