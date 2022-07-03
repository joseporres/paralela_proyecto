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
    return {adj_mat, cost};
}

t_mat blockMat(t_mat mat, int src, int dest)
{
    mat[dest][0] = INF;
    FOR (i, 0, N) mat[src][i] = INF;
    FOR (i, 0, N) mat[i][dest] = INF;
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
        FOR (dest, 0, N)
        {
            if (find(path.begin(), path.end(), dest) == path.end())
            {
                t_mat b_mat = blockMat(mat, src, dest);
                auto res = reduceMat(b_mat);
                type new_cost = (mat[src][dest] + cost + res.second);            
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
    t_mat reduce_mat = res.first;
    type cost = res.second;
    vector<int> path;
    int start = 2;
    path.push_back(start);
    clock_t beg = clock();
    BFS_BB(reduce_mat, cost, path, start);
    cout << "Time: " << double(clock()-beg)/CLOCKS_PER_SEC << endl;
    cout << "Best cost: " << upper << endl;
    cout << "Best path: " << endl;
    for (auto it : best_path) cout << districts[it] << " -> ";
    cout << districts[start] << endl;
    cout << "Best time (h): " << endl;
    cout << upper/spe << endl;
}