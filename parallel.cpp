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

void printMat(t_mat adj_mat)
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

pair<t_mat, type> reduceMat(t_mat adj_mat)
{
    type cost = 0;

    int i, j;
    // reduce row
    #pragma omp parallel
    {
        #pragma omp for private(i)
        for (i = 0; i < N; i++)
        {
            type min = adj_mat[i][0];
            #pragma omp parallel for private(j)
            for (j = 1; j < N; j++)
            {
                // #pragma omp critical
                {
                    if (adj_mat[i][j] < min)
                    {
                        min = adj_mat[i][j];
                    }
                }
            }

            if (min != INF and min != 0)
            {
                #pragma omp atomic update
                cost += min;
                #pragma omp parallel for private(j)
                for (j = 0; j < N; j++)
                {
                    adj_mat[i][j] -= min;
                }
            }
        }
    }
    // reduce col
    #pragma omp parallel
    {
        #pragma omp for private(i)
        for (i = 0; i < N; i++)
        {
            type min = adj_mat[0][i];
            #pragma omp parallel for private(j)
            for (j = 1; j < N; j++)
            {
                // #pragma omp critical
                {
                    if (adj_mat[j][i] < min)
                    {
                        min = adj_mat[j][i];
                    }
                }
            }
            if (min != INF and min != 0)
            {
                #pragma omp atomic update 
                cost += min;
                #pragma omp parallel for private(j)
                for (j = 0; j < N; j++)
                {
                    adj_mat[j][i] -= min;
                }
            }
        }
    }
    return {adj_mat, cost};
}

t_mat blockMat(t_mat mat, int src, int dest)
{
    mat[dest][0] = INF;
    int i;
    #pragma omp parallel
    {
        #pragma omp for private(i) nowait
        for (i = 0; i < N; i++)
        {
            mat[src][i] = INF;
        }
        #pragma omp for private(i)
        for (i = 0; i < N; i++)
        {
            mat[i][dest] = INF;
        }
    }
    return mat;
}

void BFS_BB(t_mat mat, type cost, vector<int> path, int src)
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
        #pragma omp parallel for private(dest)
        for (dest = 0; dest < N; dest++)
        {
            if (find(path.begin(), path.end(), dest) == path.end())
            {
                t_mat b_mat = blockMat(mat, src, dest);
                auto res = reduceMat(b_mat);
                type new_cost = (mat[src][dest] + cost + res.second);
                #pragma omp critical
                pq.push({-new_cost, {dest, res.first}});
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
    omp_set_num_threads(5);

    t_mat adj_mat(N, vector<type>(N, 0));
    fillMat(adj_mat);
    // adj_mat[0][0] = INF;
    // adj_mat[0][1] = 20;
    // adj_mat[0][2] = 30;
    // adj_mat[0][3] = 10;
    // adj_mat[0][4] = 11;
    // adj_mat[1][0] = 15;
    // adj_mat[1][1] = INF;
    // adj_mat[1][2] = 16;
    // adj_mat[1][3] = 4;
    // adj_mat[1][4] = 2;
    // adj_mat[2][0] = 3;
    // adj_mat[2][1] = 5;
    // adj_mat[2][2] = INF;
    // adj_mat[2][3] = 2;
    // adj_mat[2][4] = 4;
    // adj_mat[3][0] = 19;
    // adj_mat[3][1] = 6;
    // adj_mat[3][2] = 18;
    // adj_mat[3][3] = INF;
    // adj_mat[3][4] = 3;
    // adj_mat[4][0] = 16;
    // adj_mat[4][1] = 4;
    // adj_mat[4][2] = 7;
    // adj_mat[4][3] = 16;
    // adj_mat[4][4] = INF;

    auto res = reduceMat(adj_mat);
    t_mat reduce_mat = res.first;
    type cost = res.second;
    vector<int> path;
    int start = 0;
    path.push_back(start);
    double beg = omp_get_wtime();
    BFS_BB(reduce_mat, cost, path, start);
    cout << "Time: " << (omp_get_wtime()-beg) << endl;
    cout << "Best cost: " << upper << endl;
    cout << "Best path: " << endl;
    best_path.push_back(start);
    for (auto it : best_path) cout << it << " ";
    cout << endl;

}