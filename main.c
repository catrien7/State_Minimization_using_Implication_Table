

// Supports Moore and Mealy state minimization
// Based on Implication Table Method

//The program assumes valid user input and is not designed to handle unexpected or invalid inputs.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#define MAX_States 30
#define MAX_Inputs 10

typedef struct
{
    int s1;
    int s2;
} Dep; //Dependency

typedef struct
{
    int marked;
    int depCount;
    Dep deps[MAX_States];

} Cell;

typedef struct
{
    int numStates;
    int numInputs;

    char* states[MAX_States]; //states[max_numberOfStates]

} BASE;

typedef struct
{
    BASE base;
    int outputs[MAX_States]; //outputs[states]
    int transitions[MAX_States][MAX_Inputs]; //transitions[states][inputs]
} Moore_table;

typedef struct
{
    BASE base;
    int outputs[MAX_States][MAX_Inputs]; //outputs[states][inputs]
    int transitions[MAX_States][MAX_Inputs]; //transitions[states][inputs]

} Mealy_table;

Moore_table* init_Moore(int states, int inputs){
    Moore_table* moore= malloc(sizeof(Moore_table));
    moore->base.numStates=states;
    moore->base.numInputs=inputs;
    return moore;
}

Mealy_table* init_Mealy(int states, int inputs){
    Mealy_table* mealy= malloc(sizeof(Mealy_table));
    mealy->base.numStates=states;
    mealy->base.numInputs=inputs;
    return mealy;
}

int getStateIndex(char *states[], int n, char* name){
    for(int i=0; i<n; i++){
        if(strcmp(states[i], name) == 0) return i;
    }
    return -1;
}

Moore_table* get_Moore_table(){

    int states, inputs, i;

    printf("Enter Number of States(Maximum %d): ", MAX_States);
    scanf("%d",&states);

    printf("Enter the Number of Inputs (not the number of bits)(Maximum %d): ", MAX_Inputs);
    scanf("%d",&inputs);

    Moore_table* moore=init_Moore(states,inputs);

    for(i=0; i<states; i++){

        char name[50];
        printf("\nEnter the name of State number (%d): ", i+1);
        scanf("%s",name);
        moore->base.states[i]=malloc(strlen(name)+1);
        strcpy(moore->base.states[i], name);

    }
    for(i=0; i<states; i++){
        int j;
        for(j=0; j<inputs; j++){
            char name[50];
            printf("\nEnter the Next state of State (%s)for input number %d: ", moore->base.states[i], j);
            scanf("%s",name);
            int idx = getStateIndex(moore->base.states, states, name);
            moore->transitions[i][j] = idx;
        }
    }
    for(i=0; i<states; i++){
        int output;
        printf("\nEnter the Output for state (%s): ", moore->base.states[i]);
        scanf("%d",&output);
        moore->outputs[i]=output;
    }
    return moore;
}

Mealy_table* get_Mealy_table(){

    int states, inputs, i;

    printf("Enter Number of States(Maximum %d): ", MAX_States);
    scanf("%d",&states);

    printf("Enter the Number of Inputs (not the number of bits)(Maximum %d): ", MAX_Inputs);
    scanf("%d",&inputs);

    Mealy_table* mealy=init_Mealy(states,inputs);

    for(i=0; i<states; i++){

        char name[50];
        printf("\nEnter the name of State number (%d): ", i+1);
        scanf("%s",name);
        mealy->base.states[i]=malloc(strlen(name)+1);
        strcpy(mealy->base.states[i], name);

    }
    for(i=0; i<states; i++){
        int j;
        for(j=0; j<inputs; j++){

            char name[50];
            printf("\nEnter the Next state of state (%s) for input number (%d): ", mealy->base.states[i], j);
            scanf("%s",name);
            int idx = getStateIndex(mealy->base.states, states, name);
            mealy->transitions[i][j] = idx;
        }
    }
    for(i=0; i<states; i++){
        int j;
        for(j=0; j<inputs; j++){
            printf("\nEnter the Output of state (%s) for input number (%d): ", mealy->base.states[i], j);
            scanf("%d",&mealy->outputs[i][j]);
        }
    }
    return mealy;
}

Cell** Reduce_moore_table(Moore_table* moore){
    int i,j,y;
    int n = moore->base.numStates;
    Cell **table= (Cell**)malloc(n * sizeof(Cell*));
    table[0]=NULL;
    for(i=1; i<n; i++){
        table[i]= (Cell*)malloc(i * sizeof(Cell));
    }
    int inputs=moore->base.numInputs;
    for(i=1; i<n; i++){
        for(j=0; j<i; j++){
            table[i][j].marked=0;
            table[i][j].depCount=0;
            if(moore->outputs[j]!= moore->outputs[i]) table[i][j].marked=1; 
            if(table[i][j].marked==0){
                for(y=0; y<inputs; y++){
                    int s1= moore->transitions[j][y];
                    int s2= moore->transitions[i][y];
                    if(s1==s2) continue;
                    else if(s1>s2){
                        table[i][j].deps[table[i][j].depCount ].s1= s1;
                        table[i][j].deps[table[i][j].depCount ].s2= s2;
                        table[i][j].depCount+=1;
                    }
                    else {
                        table[i][j].deps[table[i][j].depCount ].s1= s2;
                        table[i][j].deps[table[i][j].depCount ].s2= s1;
                        table[i][j].depCount+=1;
                    }
                    
                    
                }
            }
        }
    }
    int changed=1;
    while(changed){
        changed=0;
        for(i=1; i<n; i++){
            for(j=0; j<i; j++){
                if (table[i][j].marked==0) {
                    int z;
                    for(z=0; z<table[i][j].depCount; z++){
                        int s1= table[i][j].deps[z].s1;
                        int s2= table[i][j].deps[z].s2;
                        if(s1==s2) continue;
                        if( table[s1][s2].marked==1){
                            table[i][j].marked=1;
                            changed=1;
                            break;
                        }

                    }
                }

            }
        }
    }

    return table;
}

Cell** Reduce_mealy_table(Mealy_table* mealy){
    int i,j,y;
    int n = mealy->base.numStates;
/*    Cell *table[n];
    for(i=1; i<=n; i++){
        Cell *arr[i];
        table[i]= arr[i];
    }
*/
    Cell **table= (Cell**)malloc(n * sizeof(Cell*));
    table[0]=NULL;
    for(i=1; i<n; i++){
        table[i]= (Cell*)malloc(i * sizeof(Cell));
    }
    int inputs=mealy->base.numInputs;
    for(i=1; i<n; i++){
        for(j=0; j<i; j++){
            table[i][j].marked=0;
            table[i][j].depCount=0;
            for(y=0; y<inputs; y++){
                if(mealy->outputs[j][y]!= mealy->outputs[i][y]) {table[i][j].marked=1; break;}
            }
            if(table[i][j].marked==0){
                for(y=0; y<inputs; y++){
                    int s1= mealy->transitions[j][y];
                    int s2= mealy->transitions[i][y];
                    if(s1==s2) continue;
                    else if(s1>s2){
                        table[i][j].deps[table[i][j].depCount ].s1= s1;
                        table[i][j].deps[table[i][j].depCount ].s2= s2;
                        table[i][j].depCount+=1;
                    }
                    else {
                        table[i][j].deps[table[i][j].depCount ].s1= s2;
                        table[i][j].deps[table[i][j].depCount ].s2= s1;
                        table[i][j].depCount+=1;
                    }
                    
                    
                }
            }
        }
    }
    int changed=1;
    while(changed){
        changed=0;
        for(i=1; i<n; i++){
            for(j=0; j<i; j++){
                if (table[i][j].marked==0) {
                    int z;
                    for(z=0; z<table[i][j].depCount; z++){
                        int s1= table[i][j].deps[z].s1;
                        int s2= table[i][j].deps[z].s2;
                        if(s1==s2) continue;
                        if( table[s1][s2].marked==1){
                            table[i][j].marked=1;
                            changed=1;
                            break;
                        }

                    }
                }

            }
        }
    }

    return table;
}

void print_reduced_table(Cell** table, char* states[], int n){
    int i,j;
    printf("\n\nImplication Table:\n\n");
    for(i=1; i<n; i++){
        printf("%8s", states[i]);
        for(j=0; j<n-1; j++){
            if(j>=i) printf("%8s","-");
            else{
                if(table[i][j].marked) printf("%8s","X");
                else printf("%8s","O");
            }
        }
        printf("\n");
    }
    printf("\n");
    printf("%8s","");
    for(i=0; i<n-1; i++){
        printf("%8s", states[i]);
    }

}

int find(int parent[], int x){
    if(parent[x] == x)
        return x;

    parent[x] = find(parent, parent[x]);
    return parent[x];
}

void unite(int parent[], int a,int b){
    parent[find(parent, a)] = find(parent, b);
}

int* build_state_groups(Cell** table, int n){
    int i,j;
    int* parent= malloc(n* sizeof(int));
    for(i=0; i<n; i++) parent[i]=i;
    for(i=1; i<n; i++){
        for(j=0; j<i; j++){
            if(table[i][j].marked==0){
                unite(parent, i, j);
            } 
        }
    }
    for(i=0; i<n; i++){
        parent[i] = find(parent, i);
    }
    return parent;
}

Mealy_table* build_reduced_mealy(Mealy_table* original, int parent[]){
    int i, j=0, y, num=0;
    int n=original->base.numStates, inputs=original->base.numInputs;
    int new_Index[n];
    for(i=0; i<n; i++) if(parent[i]==i) num++;
    Mealy_table* new_Mealy= init_Mealy(num, inputs);
    for(i=0; i<n; i++){
        if(parent[i]==i){
            new_Mealy->base.states[j] =malloc(strlen(original->base.states[i])+1);
            strcpy(new_Mealy->base.states[j],original->base.states[i]);
            new_Index[i]=j;
            j++;
        }
    }
    j=0;
    for(i=0; i<n; i++){
        if(parent[i]==i){
            for(y=0; y<inputs; y++){
                new_Mealy->outputs[j][y]=original->outputs[i][y];
                int index = find(parent, original->transitions[i][y]);
                new_Mealy->transitions[j][y]= new_Index[index];
            }
            j++;
        }
    }
    
    return new_Mealy;
}

Moore_table* build_reduced_moore(Moore_table* original, int parent[]){
    int i, j=0, y, num=0;
    int n=original->base.numStates, inputs=original->base.numInputs;
    int new_Index[n];
    for(i=0; i<n; i++) if(parent[i]==i) num++;
    Moore_table* new_Moore= init_Moore(num, inputs);
    for(i=0; i<n; i++){
        if(parent[i]==i){
            new_Moore->base.states[j] =malloc(strlen(original->base.states[i])+1);
            strcpy(new_Moore->base.states[j],original->base.states[i]);
            new_Moore->outputs[j]=original->outputs[i];
            new_Index[i]=j;
            j++;
        }   
    }
    j=0;
    for(i=0; i<n; i++){
        if(parent[i]==i){
            for(y=0; y<inputs; y++){
                int index = find(parent, original->transitions[i][y]);
                new_Moore->transitions[j][y]= new_Index[index];
            }
            j++;
        }
    }
    return new_Moore;
}

void print_mealy_table(Mealy_table* mealy)
{
    int i,j;

    printf("\n\n========== Mealy State Table ==========\n\n");

    printf("%8s","State");

    for(j=0; j<mealy->base.numInputs; j++){
        printf("%8s","N.S");
    }

    for(j=0; j<mealy->base.numInputs; j++){
        printf("%8s","O/P");
    }

    printf("\n");

    printf("%10s","");

    for(j=0; j<mealy->base.numInputs; j++){
        printf("( I/P%d )",j);
    }

    for(j=0; j<mealy->base.numInputs; j++){
        printf("( I/P%d )",j);
    }

    printf("\n");


    for(i=0; i<mealy->base.numStates; i++){
        printf("%8s", mealy->base.states[i]);

        for(j=0; j<mealy->base.numInputs; j++){
            printf("%8s",mealy->base.states[mealy->transitions[i][j]]);
        }

        for(j=0; j<mealy->base.numInputs; j++){
            printf("%8d",mealy->outputs[i][j]);
        }
        printf("\n");
    }

    printf("\n");
}

void print_moore_table(Moore_table* moore)
{
    int i,j;

    printf("\n\n========== Moore State Table ==========\n\n");

    printf("%8s","State");

    for(j=0; j<moore->base.numInputs; j++){
        printf("%8s","N.S");
    }

    printf("%8s","O/P");

    printf("\n");
    printf("%10s","");

    for(j=0; j<moore->base.numInputs; j++){
        printf("( I/P%d )",j);
    }

    printf("\n");

    for(i=0; i<moore->base.numStates; i++){
        printf("%8s", moore->base.states[i]);

        for(j=0; j<moore->base.numInputs; j++){
            printf("%8s",moore->base.states[moore->transitions[i][j]]);
        }

        printf("%8d",moore->outputs[i]);
        printf("\n");
    }

    printf("\n");
}

void free_mealy(Mealy_table* mealy){
    int i;
    for(i=0; i<mealy->base.numStates; i++){
        free(mealy->base.states[i]);
    }
    free(mealy);
}

void free_moore(Moore_table* moore){
    int i;
    for(i=0; i<moore->base.numStates; i++){
        free(moore->base.states[i]);
    }
    free(moore);
}

void free_table(Cell** table, int n){
    int i;
    for(i=0; i<n; i++){
        free(table[i]);
    }
    free(table);
}

int main(){
    while(1){
        int type;
        printf("Choose a number:\n");
        printf("1. Moore\n");
        printf("2. Mealy\n");
        scanf("%d",&type);

        switch (type){
            case 1: {
                Moore_table* moore = get_Moore_table();
                print_moore_table(moore);
                Cell** table = Reduce_moore_table(moore);
                print_reduced_table(table, moore->base.states, moore->base.numStates);
                int* parent = build_state_groups(table, moore->base.numStates);
                Moore_table* reduced = build_reduced_moore(moore, parent);
                print_moore_table(reduced);
                free(parent);
                free_table(table,moore->base.numStates);
                free_moore(moore);
                free_moore(reduced);
                break;
            }

            case 2: {
                Mealy_table* mealy = get_Mealy_table();
                print_mealy_table(mealy);
                Cell** table = Reduce_mealy_table(mealy);
                print_reduced_table(table, mealy->base.states, mealy->base.numStates);
                int* parent = build_state_groups(table, mealy->base.numStates);
                Mealy_table* reduced = build_reduced_mealy(mealy, parent);
                print_mealy_table(reduced);
                free(parent);
                free_table(table,mealy->base.numStates);
                free_mealy(mealy);
                free_mealy(reduced);
                break;
            }

            default:
                printf("Invalid choice\n");
                continue;

        }
        char c;
        printf("\nDo you want to continue?(Y/N)\n");
        scanf(" %c",&c);
        if(c!='Y' && c!='y') break;

    }

    return 0;
}
