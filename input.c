#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//TODO
/*
CSV input file
*/
int nodos;
int** matrix;

void create_matrix(){
    
    do{
        system("clear");
        printf("Ingrese numero de nodos mayor a 1: ");
        scanf("%d",&nodos); 
    }while(nodos <= 1);

    matrix = (int **)malloc (nodos*sizeof(int *));

    for (int i=0;i<nodos;i++)
        matrix[i] = (int *) malloc (nodos*sizeof(int));

    for(int i = 0; i < nodos; i++){
        for(int j = i; j < nodos; j++){
            if( i == j){
                matrix[i][j] = 0;
            }
            else{
                printf("Ingrese valor para la posicion (%d,%d): ",i,j);
                scanf("%d",&matrix[i][j]);
                matrix[j][i] = matrix[i][j];
            }
        }
    }

}

void display(){
    system("clear");
    printf("Matriz de adyacencia:\n");
    for(int i = 0; i < nodos; i++){
        for(int j = 0; j < nodos; j++){
            printf("%d ",matrix[i][j]);
        }
        printf("\n");
    } 
}

void readCsv(){
    
}

int main(){
    int opcion_menu, cont = 0;
    while(1){
        if(cont != 0){
            printf("\n");
            printf("Presione una tecla para continuar...\n");
            getchar();
            getchar();
        }
        system("clear");
        printf("MENU DE OPCIONES: \n");
        printf("1. Crear matriz\n");
        printf("2. Subir csv\n");
        printf("3. Mostrar matriz\n");
        printf("4. Exit\n");

        scanf("%d",&opcion_menu);
        printf("\n");
        switch(opcion_menu){
            case 1:
                create_matrix();
                break;
            case 2:
                //subir csv
                break;
            case 3:
                display();
                break;
            case 4:
                return 0;
            default:
                printf("Opcion no valida\n");
                break;
        }
        cont++;
        
    }




}