#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>
#include <omp.h>
#include <ctime>
#include <sw/redis++/redis++.h>

using namespace std;
using namespace sw::redis;

int N = 100;
int n_thr = 4;


double len(vector<double>& v1){
    double l = 0;
    #pragma omp parallel for num_threads(n_thr) shared(l)
    for(int i = 0; i < N; i++){
        l += v1[i]*v1[i];
    }
    l = sqrt(l);
    return l;
}

vector<double> norm (vector<double> v1){
    double l = len(v1);
    #pragma omp parallel for num_threads(n_thr) shared(l)
    for(int i = 0; i < N; i++){
        v1[i] = v1[i]/l;
    }
    return v1;
}

double scalar(vector<double>& v1, vector<double>& v2){
    double sc = 0;
    #pragma omp parallel for num_threads(n_thr) shared(sc)
    for(int i = 0; i < N; i++){
        sc += v1[i]*v2[i];
    }
    return sc;
}

vector<double> proec(vector<double>& v1, vector<double>& v2){
    vector<double> pr_v1(N);
    double koef = scalar(v1, v2) / scalar(v2, v2);
    #pragma omp parallel for num_threads(n_thr) shared(pr_v1)
    for(int i = 0; i < N; i++){
        pr_v1[i] = koef * v2[i];
    }
    return pr_v1;
};

vector<double> raz(vector<double>& v1, vector<double>& v2){
    vector<double> r(N);
    #pragma omp parallel for num_threads(n_thr) shared(r)
    for(int i = 0; i < N; i++){
        r[i] = v1[i] - v2[i];
    }
    return r;
}

int main(){
    //ifstream fin("Vectors.txt");
    auto redis_in = Redis("tcp://127.0.0.1:6379/inp");
    string value;
    vector <double> v[N];
    for(int i = 0; i < N; i++){
        v[i].reserve(N);
    }
    vector <double> answ[N];
    for(int i = 0; i < N; i++){
        answ[i].reserve(N);
    }
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            //fin >> v[i][j];
            auto val = redis_in.get(to_string(i)+' '+to_string(j));
            value = *val;
            v[i][j] = stod(value);
            //cout << v[i][j] << ' ';
        }
        //cout << endl;
    }
    //fin.close();
    ///cout << v[0] << " " << v[1023][0];
    int c_norm_v = 0;
    double start_time =  omp_get_wtime();
    double l = len(v[0]);
    for(int i = 0; i < N; i++){
        answ[0][i] = v[0][i] / l; 
    }
    c_norm_v++;
    vector <double> buff(N);
    for(int i = 1; i < N; i++){
        buff = proec(v[i], answ[0]);
        answ[i] = raz(v[i], buff);
        for(int j = 1; j < c_norm_v; j++){
            buff = proec(v[i], answ[j]);
            answ[i] = raz(answ[i], buff);
        }
        c_norm_v++;
        answ[i] = norm(answ[i]);
    }
    //ofstream fout("Baza.txt");
    auto redis_out = Redis("tcp://127.0.0.1:6379/out");
    string key;
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            //fout << answ[i][j] << ' ';
            key = to_string(i) + ' ' + to_string(j);
            value = to_string(v[i][j]);
            redis_out.set(key,value);
        }
        //fout << '\n';
    }
    //fout.close();
    double end_time = omp_get_wtime();
    double search_time = end_time - start_time;
    cout << search_time << endl;
    return 0;
}