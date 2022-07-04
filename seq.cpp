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

int N = 10;
type INF = (1.0/0.0);
type upper = INF;
vector<int> best_path;
double spe = 5;

#define FOR(i,a,b) for (int (i) = (a); (i) < (b); ++(i))
#define FOR2(i,a,b,c) for (int (i) = (a); (i) < (b); (i) += (c))

using t_mat = vector<vector<type>>;

vector<string> districts = {"Lima Centro", "Lince", "Miraflores", 
                            "Barranco", "Rimac", "Los Olivos", "La Molina", 
                            "La Victoria", "Magdalena", "San Borja"};

void printMat(t_mat &adj_mat)
{
    cout << "{";
    FOR(i, 0, N)
    {
        cout << "{";
        FOR(j, 0, N)
        {
            printf("%f, ", adj_mat[i][j]);
        }
        printf("}\n");
    }
    printf("}\n");
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
    mat[dest][src] = INF;
    FOR (i, 0, N) 
    {
        mat[src][i] = INF;
        mat[i][dest] = INF;
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
        FOR (dest, 0, N)
        {
            if (find(path.begin(), path.end(), dest) == path.end())
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
            BFS_BB(top.second.second, -top.first, path, top.second.first);
            path.pop_back();
        }
    }
    // else
    // {
    //     cout << "BOUNDED " << cost << endl;
    // }
}

void finish()
{
    printf("\n");
    printf("Presione una tecla para continuar...\n");
    getchar();
    getchar();
}

void distanceMatrix(t_mat &adj_mat, bool flag = true)
{
    N = 10;
    adj_mat = {
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
    if (flag) finish();
}

t_mat createMatrix()
{
    system("clear");
    do{
        printf("Ingrese numero de nodos mayor a 1: ");
        scanf("%d",&N); 
    }while(N <= 1);

    t_mat matrix(N, vector<type>(N, 0));

    for (int i = 0; i < N; i++){
        for (int j = i; j < N; j++){
            if( i == j){
                matrix[i][j] = INF;
            }
            else{
                printf("Ingrese valor para la posicion (%d,%d): ",i,j);
                scanf("%lf",&matrix[i][j]);
                matrix[j][i] = matrix[i][j];
            }
        }
    }
    return matrix;
}

void oneTSP(t_mat adj_mat)
{
    upper = INF;
    type res = reduceMat(adj_mat);
    vector<int> path;

    int start;
    system("clear");
    do{
        printf("Ingrese numero de nodo para iniciar el TSP: ");
        scanf("%d", &start); 
    }while(start < 0 || start >= N);

    path.push_back(start);
    BFS_BB(adj_mat, res, path, start);
    cout << "Best cost: " << upper << endl;
    cout << "Best path: " << endl;
    for (auto it : best_path) cout << it << " -> ";
    cout << start << endl;
    cout << "Time to travel in hours: " << endl;
    cout << upper/spe << endl;

    finish();
}

void allTSP(t_mat adj_mat)
{
    system("clear");
    t_mat aux_mat;
    FOR (start, 0, N)
    {
        upper = INF;
        aux_mat = adj_mat;
        type res = reduceMat(adj_mat);
        vector<int> path;
        path.push_back(start);
        BFS_BB(adj_mat, res, path, start);
        cout << "Best cost: " << upper << endl;
        cout << "Best path: " << endl;
        for (auto it : best_path) cout << it << " -> ";
        cout << start << endl;
        cout << "Time to travel in hours: " << endl;
        cout << upper/spe << endl;
        adj_mat = aux_mat;
    }
    finish();
}

int main(int argc, char *argv[])
{
    int opcion_menu;
    t_mat adj_mat;
    distanceMatrix(adj_mat, false);
    while(1)
    {
        system("clear");
        printf("MENÚ DE OPCIONES: \n");
        printf("1. Utilizar matriz existente\n");
        printf("2. Ingresar nueva matriz\n");
        printf("3. Calcular TSP desde un punto de inicio\n");
        printf("4. Calcular TSP desde todos los puntos de inicio\n");
        printf("5. Salir\n");

        scanf("%d",&opcion_menu);
        printf("\n");
        switch(opcion_menu){
            case 1:
                distanceMatrix(adj_mat);
                break;
            case 2:
                adj_mat = createMatrix();
                break;
            case 3:
                oneTSP(adj_mat);
                break;
            case 4:
                allTSP(adj_mat);
                break;
            case 5:
                return 0;
            default:
                printf("Opción no válida\n");
                break;
        }
    }
}