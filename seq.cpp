#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <vector>
#include <queue>
#include <algorithm>
#include <ctime>

using namespace std;

typedef double type;

// matriz de ejemplo Abdul Bari
// https://www.youtube.com/watch?v=1FEP_sNb62k

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

    // reduce row
    FOR (i, 0, N)
    {
        type min = adj_mat[i][0];
        FOR (j, 1, N)
        {
            if (adj_mat[i][j] < min) min = adj_mat[i][j];
        }
        if (min != INF and min != 0)
        {
            cost += min;
            FOR (j, 0, N)
            {
                adj_mat[i][j] -= min;
            }
        }
    }
    // reduce col
    FOR (i, 0, N)
    {
        type min = adj_mat[0][i];
        FOR (j, 1, N)
        {
            if (adj_mat[j][i] < min) min = adj_mat[j][i];
        }
        if (min != INF and min != 0)
        {
            cost += min;
            FOR (j, 0, N)
            {
                adj_mat[j][i] -= min;
            }
        }
    }
    return cost;
    // return {adj_mat, cost};
}

t_mat blockMat(t_mat mat, int &src, int &dest)
{
    mat[dest][0] = INF;
    FOR (i, 0, N) mat[src][i] = INF;
    FOR (i, 0, N) mat[i][dest] = INF;
    return mat;
}

void BFS_BB(t_mat &mat, type cost, vector<int> &path, int src, vector<bool> &visited)
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
        FOR (dest, 0, N)
        {
            if (!visited[dest])
            {
                t_mat b_mat = blockMat(mat, src, dest);
                type res = reduceMat(b_mat);
                type new_cost = (mat[src][dest] + cost + res);            
                pq.push({-new_cost, {dest, b_mat}});
            }
        }
        while (!pq.empty())
        {
            auto top = pq.top();
            pq.pop();
            path.push_back(top.second.first);
            visited[top.second.first] = true;
            BFS_BB(top.second.second, -top.first, path, top.second.first, visited);
            path.pop_back();
            visited[top.second.first] = false;
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

    // t_mat adj_mat = {
    //     {INF,   4.6,    9,      11.3,   3.8,    12.3,   15.3,   4.9,    6.3,    8.8},
    //     {4.6,   INF,    4.4,    6.9,    7.4,    16.8,   13.9,   2.8,    4.6,    5.6},
    //     {9,     4.4,    INF,    2.7,    11.8,   21.3,   13.9,   6.5,    6.5,    6.1},
    //     {11.3,  6.9,    2.7,    INF,    14.1,   23.5,   14.5,   8.3,    8.7,    7.4},
    //     {3.8,   7.4,    11.8,   14.1,   INF,    11.4,   16,     6.6,    9.6,    10.3},
    //     {12.3,  16.8,   21.3,   23.5,   11.4,   INF,    26.4,   16.4,   16.5,   20.2},
    //     {15.3,  13.9,   13.9,   14.5,   16,     26.4,   INF,    11.7,   17.9,   8.8},
    //     {4.9,   2.8,    6.5,    8.3,    6.6,    16.4,   11.7,   INF,    6.8,    4.3},
    //     {6.3,   4.6,    6.5,    8.7,    9.6,    16.5,   17.9,   6.8,    INF,    9.2},
    //     {8.8,   5.6,    6.1,    7.4,    10.3,   20.2,   8.8,    4.3,    9.2,    INF}};

    t_mat adj_mat = {
        {INF, 5.3, 10, 11.8, 3.5, 12.5, 21.4, 6.5, 6.9, 10.3},
        {5.3, INF, 4.3, 7.7, 7.7, 17.5, 13.6, 2.8, 5.2, 6},
        {10, 4.3, INF, 2.9, 13.9, 23.7, 16.6, 8.3, 7.1, 9},
        {11.8, 7.7, 2.9, INF, 15, 24.8, 17.7, 9.4, 9.8, 10.1},
        {3.5, 7.7, 13.9, 15, INF, 14.3, 21.5, 9.3, 9.9, 13.1},
        {12.5, 17.5, 23.7, 24.8, 14.3, INF, 31.7, 18, 19, 27.9},
        {21.4, 13.6, 16.6, 17.7, 21.5, 31.7, INF, 14.6, 17.3, 9.3},
        {6.5, 2.8, 8.3, 9.4, 9.3, 18, 14.6, INF, 8.8, 4.7},
        {6.9, 5.2, 7.1, 9.8, 9.9, 19, 17.3, 8.8, INF, 9.5},
        {10.3, 6, 9, 10.1, 13.1, 27.9, 9.3, 4.7, 9.5, INF}};


    vector<bool> visited(N, 0);
    double spe = 5;

    type res = reduceMat(adj_mat);
    vector<int> path;
    int start = 1;
    path.push_back(start);
    visited[start] = 1;
    clock_t beg = clock();
    BFS_BB(adj_mat, res, path, start, visited);
    cout << "Time: " << double(clock()-beg)/CLOCKS_PER_SEC << endl;
    cout << "Best cost: " << upper << endl;
    cout << "Best path: " << endl;
    for (auto it : best_path) cout << districts[it] << " -> ";
    cout << districts[start] << endl;
    cout << "Best time (h): " << endl;
    cout << upper/spe << endl;
}