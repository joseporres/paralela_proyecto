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

void fillMatRand(t_mat &mat)
{
    srand(time(0));
    FOR(i, 0, N)
    {
        FOR(j, 0, N)
        {
            mat[i][j] = (rand()%100)+20;
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
    // fillMatRand(adj_mat);

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

    t_mat adj_mat = {{INF, 865.000000, 656.000000, 425.000000, 854.000000, 327.000000, 100.000000, 206.000000, 516.000000, 663.000000, 979.000000, 78.000000, 132.000000, 156.000000, 498.000000, 296.000000, 243.000000 },
                        {865.000000, INF, 764.000000, 602.000000, 550.000000, 843.000000, 694.000000, 278.000000, 812.000000, 25.000000, 891.000000, 947.000000, 659.000000, 942.000000, 954.000000, 480.000000, 472.000000 },
                        {656.000000, 764.000000, INF, 615.000000, 972.000000, 631.000000, 803.000000, 37.000000, 657.000000, 494.000000, 555.000000, 943.000000, 237.000000, 976.000000, 658.000000, 338.000000, 583.000000 },
                        {425.000000, 602.000000, 615.000000, INF, 330.000000, 572.000000, 193.000000, 385.000000, 516.000000, 439.000000, 416.000000, 136.000000, 993.000000, 694.000000, 115.000000, 428.000000, 449.000000 },
                        {854.000000, 550.000000, 972.000000, 330.000000, INF, 180.000000, 648.000000, 777.000000, 86.000000, 59.000000, 862.000000, 186.000000, 265.000000, 781.000000, 854.000000, 916.000000, 71.000000 },
                        {327.000000, 843.000000, 631.000000, 572.000000, 180.000000, INF, 274.000000, 34.000000, 972.000000, 888.000000, 607.000000, 839.000000, 901.000000, 437.000000, 248.000000, 622.000000, 108.000000 },
                        {100.000000, 694.000000, 803.000000, 193.000000, 648.000000, 274.000000, INF, 43.000000, 701.000000, 545.000000, 665.000000, 462.000000, 239.000000, 284.000000, 173.000000, 100.000000, 308.000000 },
                        {206.000000, 278.000000, 37.000000, 385.000000, 777.000000, 34.000000, 43.000000, INF, 897.000000, 975.000000, 246.000000, 239.000000, 59.000000, 321.000000, 860.000000, 675.000000, 676.000000 },
                        {516.000000, 812.000000, 657.000000, 516.000000, 86.000000, 972.000000, 701.000000, 897.000000, INF, 54.000000, 443.000000, 189.000000, 654.000000, 408.000000, 254.000000, 265.000000, 388.000000 },
                        {663.000000, 25.000000, 494.000000, 439.000000, 59.000000, 888.000000, 545.000000, 975.000000, 54.000000, INF, 754.000000, 972.000000, 7.000000, 292.000000, 929.000000, 484.000000, 662.000000 },
                        {979.000000, 891.000000, 555.000000, 416.000000, 862.000000, 607.000000, 665.000000, 246.000000, 443.000000, 754.000000, INF, 9.000000, 404.000000, 385.000000, 355.000000, 572.000000, 341.000000 },
                        {78.000000, 947.000000, 943.000000, 136.000000, 186.000000, 839.000000, 462.000000, 239.000000, 189.000000, 972.000000, 9.000000, INF, 144.000000, 898.000000, 665.000000, 798.000000, 877.000000 },
                        {132.000000, 659.000000, 237.000000, 993.000000, 265.000000, 901.000000, 239.000000, 59.000000, 654.000000, 7.000000, 404.000000, 144.000000, INF, 534.000000, 138.000000, 972.000000, 16.000000 },
                        {156.000000, 942.000000, 976.000000, 694.000000, 781.000000, 437.000000, 284.000000, 321.000000, 408.000000, 292.000000, 385.000000, 898.000000, 534.000000, INF, 355.000000, 290.000000, 845.000000 },
                        {498.000000, 954.000000, 658.000000, 115.000000, 854.000000, 248.000000, 173.000000, 860.000000, 254.000000, 929.000000, 355.000000, 665.000000, 138.000000, 355.000000, INF, 463.000000, 521.000000 },
                        {296.000000, 480.000000, 338.000000, 428.000000, 916.000000, 622.000000, 100.000000, 675.000000, 265.000000, 484.000000, 572.000000, 798.000000, 972.000000, 290.000000, 463.000000, INF, 664.000000 },
                        {243.000000, 472.000000, 583.000000, 449.000000, 71.000000, 108.000000, 308.000000, 676.000000, 388.000000, 662.000000, 341.000000, 877.000000, 16.000000, 845.000000, 521.000000, 664.000000, INF }};

    double spe = 5;

    auto res = reduceMat(adj_mat);
    vector<int> path;
    int start = 0;
    path.push_back(start);
    // double beg = omp_get_wtime();
    BFS_BB(adj_mat, res, path, start);
    // cout << "Time: " << (omp_get_wtime()-beg) << endl;
    cout << "Best cost: " << upper << endl;
    cout << "Best path: " << endl;
    for (auto it : best_path) cout << it << " -> ";
    cout << start << endl;
    cout << "Time to travel in hours: " << endl;
    cout << upper/spe << endl;
}