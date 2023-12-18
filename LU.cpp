#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include "mpi.h"

using namespace std;

void LU_raz(int start, int n, vector <double>* A, vector <double> * L, vector <double> * U, int argc, char **argv){
    if ((n - start) > 0){
        double u1 = A[0][0];
        double l1 = 1;
        vector <double> R_u(n - start - 1);
        vector <double> C_l(n - start - 1);
        //vector <double> l_L_U_[n - start - 1];
        //vector <double> g_L_U_[n - start - 1];
        vector <double> L_U_[n - start - 1];
        vector <double> umn[n - start - 1];
        for(int i = 0; i < n-start-1; i++){
            //l_L_U_[i].reserve(n-start-1);
            //g_L_U_[i].reserve(n-start-1);
            L_U_[i].reserve(n-start-1);
            umn[i].reserve(n-start-1);
        }
        L[start][start] = l1;
        U[start][start] = u1;
        for(int i = 1; i < n - start; i++){
            R_u[i-1] = A[0][i]; 
            C_l[i-1] = A[i][0] / u1;
        }
        int rank, size;
        MPI_Status status;
        MPI_Comm_size(MPI_COMM_WORLD, &size);
	    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        //Параллельный расчёт
        for(int i = rank; i < n -start - 1; i += size){
            for(int j = 0; j < n-start-1; j ++){
                umn[i][j] = R_u[j] * C_l[i];
                //l_L_U_[i][j] = A[i+1][j+1] - umn[i][j];
                L_U_[i][j] = A[i+1][j+1] - umn[i][j];
            }
        }

        //Отправка сообщений от всех всем
        for(int k = 0; k < size; k++){
            for(int i = k; i < n -start - 1; i +=size){
                for(int j = 0; j < n-start-1; j++){
                    MPI_Bcast(&L_U_[i][j], 1, MPI_DOUBLE, k, MPI_COMM_WORLD);
                }
            }
            MPI_Barrier(MPI_COMM_WORLD);
        }

        /*for(int i = 1; i < n -start - 1; i += size){
            for(int j = 0; j < n-start-1; j++){
                MPI_Bcast(&L_U_[i][j], 1, MPI_DOUBLE, 1, MPI_COMM_WORLD);
            }
        }*/

        /*for(int i = 0; i < n -start - 1; i ++){
            for(int j = 0; j < n-start-1; j ++){
                MPI_Allreduce(&l_L_U_[i][j], &g_L_U_[i][j], 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
            }
        }*/
        //MPI_Barrier(MPI_COMM_WORLD);
        /*cout << "-------------proc "<< rank <<"\n";
        for(int i = 0; i < n-start-1; i++){
            for(int j = 0; j < n-start-1; j++){
                cout << L_U_[i][j] << ' ';
            }
            cout << endl;
        }*/
        /*for(int i = 0; i < n- start - 1; i++){
            buff = L_U_[i][0];
            cout << "proc " << rank << "LU " << L_U_[i][0] << endl;
            if(rank == 0) MPI_Reduce(MPI_IN_PLACE,&L_U_[i],n-start-1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
            else MPI_Reduce(&L_U_[i],&L_U_[i],n-start-1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
        }
        if (rank < (n -start - 1)){
            for(int j = 0; j < n-start-1; j++){
                L_U_[rank][j] = A[rank+1][j+1] - umn[rank][j];
            }
        }*/
        for(int i = start + 1; i < n; i++){
            L[i][start] = C_l[i - start - 1];
            U[start][i] = R_u[i - start - 1];
        }
        
        LU_raz(start+1,n,L_U_,L,U, argc, argv);
    }
}

int main(int argc, char **argv){
    int n = 100;
    //cin >> n;
    ifstream fin("Vectors.txt");
    vector <double> a[n];
    for(int i = 0; i < n; i++){
        a[i].reserve(n);
        for(int j = 0; j < n; j++){
            fin >> a[i][j];
        }
    }
    fin.close();
    vector <double> L[n];
    vector <double> U[n];
    for(int i = 0; i < n; i++){
        L[i].reserve(n);
        U[i].reserve(n);
        for(int j = 0; j < n; j++){
            L[i][j] = 0;
            U[i][j] = 0;
        }
    }
    int c_norm_v = 0;
    double start_time =  clock();
    MPI_Init(&argc, &argv);
    LU_raz(0, n, a, L, U, argc, argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(rank == size-1){
        /*cout << "-------------\n";
        for(int i = 0; i < n; i++){
            for(int j = 0; j < n; j++){
                cout << L[i][j] << ' ';
            }
            cout << endl;
        }
        cout << "-------------\n";
        for(int i = 0; i < n; i++){
            for(int j = 0; j < n; j++){
                cout << U[i][j] << ' ';
            }
            cout << endl;
        }*/
        double end_time = clock();
        double search_time = end_time - start_time;
        cout << search_time << endl;
    }
    MPI_Finalize();
    return 0;
}