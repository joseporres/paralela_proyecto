#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <vector>
#include <queue>
#include <algorithm>
#include <omp.h>

using namespace std;

typedef double type;

const int N = 10;
const int NUM_THREADS = 2;
int myid, numprocs;
type global_min_cost;
type INF = (1.0/0.0);
type upper = INF;
vector<int> best_path;

#define FOR(i,a,b) for (int (i) = (a); (i) < (b); ++(i))
#define FOR2(i,a,b,c) for (int (i) = (a); (i) < (b); (i) += (c))

using t_mat = vector<vector<type>>;

void fillMat(t_mat &mat)
{
    FOR(i, 0, N)
    {
        FOR(j, 0, N)
        {
            mat[i][j] = (i+j);
        }
    }
}

void printMat(t_mat &adj_mat)
{
    FOR(i, 0, N)
    {
        FOR(j, 0, N)
        {
            printf("%f ", adj_mat[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

type reduceMat(t_mat &adj_mat)
{
    type cost = 0;

    int i, j;
    // reduce row
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        #pragma omp for private(i, j) reduction(+:cost)
        for (i = 0; i < N; i++)
        {
            type min = adj_mat[i][0];
            for (j = 1; j < N; j++)
            {
                if (adj_mat[i][j] < min)
                {
                    min = adj_mat[i][j];
                }
            }

            if (min != INF and min != 0)
            {
                // #pragma omp atomic update
                cost += min;
                for (j = 0; j < N; j++)
                {
                    adj_mat[i][j] -= min;
                }
            }
        }
    }
    // reduce col
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        #pragma omp for private(i, j) reduction(+:cost)
        for (i = 0; i < N; i++)
        {
            type min = adj_mat[0][i];
            for (j = 1; j < N; j++)
            {
                if (adj_mat[j][i] < min)
                {
                    min = adj_mat[j][i];
                }
            }
            if (min != INF and min != 0)
            {
                // #pragma omp atomic update 
                cost += min;
                for (j = 0; j < N; j++)
                {
                    adj_mat[j][i] -= min;
                }
            }
        }
    }
    return cost;
    // return {adj_mat, cost};
}

t_mat blockMat(t_mat mat, int &src, int &dest)
{
    mat[dest][src] = INF;
    int i;
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        #pragma omp for private(i)
        for (i = 0; i < N; i++)
        {
            mat[src][i] = INF;
            mat[i][dest] = INF;
        }
    }
    return mat;
}

void BFS_BB(t_mat &mat, type cost, vector<int> &path, int src)
{
    if (path.size() == N)
    {
        if (cost < upper)
        {
            upper = cost;
            best_path = path;
        }
        return;
    }
    if (cost < upper)
    {
        priority_queue<pair<type, pair<int, t_mat>>> pq;
        int dest;
        #pragma omp parallel for private(dest, path) num_threads(NUM_THREADS)
        for (dest = 0; dest < N; dest++)
        {
            if (find(path.begin(), path.end(), dest) == path.end())
            {
                t_mat b_mat = blockMat(mat, src, dest);
                auto res = reduceMat(b_mat);
                type new_cost = (mat[src][dest] + cost + res);  
                #pragma omp critical          
                pq.push({-new_cost, {dest, b_mat}});
            }
        }
        while (!pq.empty())
        {
            auto top = pq.top();
            pq.pop();
            path.push_back(top.second.first);
            BFS_BB(top.second.second, -top.first, path, top.second.first);
            path.pop_back();
        }
    }
    // else
    // {
    //     cout << "BOUNDED " << cost << endl;
    // }
}

int main(int argc, char *argv[])
{
    // t_mat adj_mat(N, vector<type>(N, 0));
    // fillMat(adj_mat);

    vector<string> districts = {"Lima Centro", "Lince", "Miraflores", 
                                "Barranco", "Rimac", "Los Olivos", "La Molina", 
                                "La Victoria", "Magdalena", "San Borja"};

    t_mat adj_mat = {
        {INF,   4.6,    9,      11.3,   3.8,    12.3,   15.3,   4.9,    6.3,    8.8},
        {4.6,   INF,    4.4,    6.9,    7.4,    16.8,   13.9,   2.8,    4.6,    5.6},
        {9,     4.4,    INF,    2.7,    11.8,   21.3,   13.9,   6.5,    6.5,    6.1},
        {11.3,  6.9,    2.7,    INF,    14.1,   23.5,   14.5,   8.3,    8.7,    7.4},
        {3.8,   7.4,    11.8,   14.1,   INF,    11.4,   16,     6.6,    9.6,    10.3},
        {12.3,  16.8,   21.3,   23.5,   11.4,   INF,    26.4,   16.4,   16.5,   20.2},
        {15.3,  13.9,   13.9,   14.5,   16,     26.4,   INF,    11.7,   17.9,   8.8},
        {4.9,   2.8,    6.5,    8.3,    6.6,    16.4,   11.7,   INF,    6.8,    4.3},
        {6.3,   4.6,    6.5,    8.7,    9.6,    16.5,   17.9,   6.8,    INF,    9.2},
        {8.8,   5.6,    6.1,    7.4,    10.3,   20.2,   8.8,    4.3,    9.2,    INF}};

    double spe = 5;

    auto res = reduceMat(adj_mat);
    vector<int> path;
    int start = 2;
    path.push_back(start);
    double beg = omp_get_wtime();
    BFS_BB(adj_mat, res, path, start);
    cout << "Time: " << (omp_get_wtime()-beg) << endl;
    cout << "Best cost: " << upper << endl;
    cout << "Best path: " << endl;
    for (auto it : best_path) cout << districts[it] << " -> ";
    cout << districts[start] << endl;
    cout << "Best time (h): " << endl;
    cout << upper/spe << endl;
}