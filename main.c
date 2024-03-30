#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

FILE *cities; //file read
int numberOfCities = 0; //Number of cities
FILE *fOutPtr; //file out
int loaded = 0; //did loading of file or not

struct node{
    int weight;
};

typedef struct node* edge;
//-------------------------------------------------------------------------

int countlines(FILE *filename){
    // count the number of lines in the file called filename
    int ch=0;
    int lines=1;

    if (filename == NULL){
        return 0;
    }

    while ((ch = fgetc(filename)) != EOF){ //read till the end of file
        if (ch == '\n') //if newline lines++
            lines++;
    }
    fclose(filename);
    return lines;
}

char *trimwhitespace(char *str){ //this function used to trim all spaces from the words to that strcmp work properly
    char *end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}

int least(int dist[], int visited[]) { //find the minimum distance
    int min = 99999;
    int indexOfMin;

    for (int v=0; v<numberOfCities; v++)
    {
        if (visited[v] == 0 && dist[v] <= min)
            min = dist[v], indexOfMin = v;
    }

    return indexOfMin;
}

void loadInfo(edge graph[][numberOfCities], char cities_names[][100]){

    loaded = 1;

    //initialing nodes----------------------------------------------------
    for(int i=0; i<numberOfCities; i++) {
        for(int j=0; j<numberOfCities; j++) {
            graph[i][j] = (edge)malloc(sizeof(struct node));
            graph[i][j]->weight=0;
        }
    }

    //File reading------------------------------------------------------
    FILE *cities3 = fopen("Cities.txt", "r");
    if(cities3 == NULL) { //if file not found
        printf("file Cities.txt cannot be found");
        exit(1);
    }

    char line[200];
    int i, j;

    while(fgets(line, 200, cities3)) {
        char city_name[100];
        char to[70];
        int cost;

        char *token;
        token = strtok(line, " ");
        strcpy(city_name, trimwhitespace(token));

        for(int k=0; k<numberOfCities; k++){ //to know the index of the city
            if(strcmp(cities_names[k], city_name) == 0){
                i = k;
                break;
            }
        }

        token = strtok(NULL, " ");
        strcpy(to, trimwhitespace(token));

        for(int k=0; k<numberOfCities; k++) { //to know the index of the city
            if (strcmp(cities_names[k], to) == 0) {
                j = k;
                break;
            }
        }

        token = strtok(NULL, " ");
        cost = atoi(token);

        graph[i][j]->weight=cost;
        graph[j][i]->weight=cost; //bcz it's indirect
    }
    fclose(cities3);
}

void printResult(char cities_names[][100], int j, int prev[]){ //print all parents of a node,as previous nodes.
    if(prev[j]==-1){ //if no parent
        return;
    }
    printResult(cities_names, prev[j], prev); //to get back to the very first parent and move to the next ones
    printf("|\t%-14s\t|\t %-13s\t|\n", cities_names[prev[j]], cities_names[j]);
}

void PrintResult2(char cities_names[][100], int j, int prev[]) { //print all parents of a node,as previous nodes. but in FILE.
    if(prev[j]==-1){ //if no parent
        return;
    }
    PrintResult2(cities_names, prev[j], prev); //to get back to the very first parent and move to the next ones
    fprintf(fOutPtr, "|\t\t%-14s\t|\t\t %-13s\t|\n", cities_names[prev[j]], cities_names[j]);
}

void Dijkstra(edge graph[][numberOfCities], char cities_names[][100], int from, int to) {
    int visited[numberOfCities];
    int dist[numberOfCities];
    int prev[numberOfCities];

    for(int i=0; i<numberOfCities; i++) {
        visited[i]=0, dist[i]=99999, prev[i]=-1; //initialize infinity and no parents and not visited
    }
    dist[from]=0;

    for(int i=0; i<numberOfCities; i++) {
        int u = least(dist, visited);
        if(dist[u]==99999) {
            break;
        }
        visited[u]=1;
        for(int j=0; j<numberOfCities; j++) {
            if (visited[j]==0 && graph[u][j]->weight !=0 && dist[u] != 99999 && dist[u] + graph[u][j]->weight < dist[j]) {
                dist[j] = dist[u] + graph[u][j]->weight;
                prev[j]=u;
            }

        }
    }

    fOutPtr = fopen("shortest_path.txt", "a");
    if(visited[to]!=0) {
        if(dist[to] != 0){
            //On screen ----------------------------------------------------------------------
            printf("\nThe following path is followed:\n\n");
            printf("=================================================\n");
            printf("|\t  From\t\t|\t   To   \t|\n");
            printf("-------------------------------------------------\n");
            printResult(cities_names, to, prev);
            printf("=================================================\n");
            printf("|   The cost from %s to %s  =   %d\t|\n", cities_names[from], cities_names[to], dist[to]);
            printf("=================================================\n\n");
            //In the file --------------------------------------------------------------------
            fprintf(fOutPtr, "\nThe following path is followed:\n\n");
            fprintf(fOutPtr, "=================================================\n");
            fprintf(fOutPtr, "|\t\t  From\t\t\t|\t\t   To   \t\t|\n");
            fprintf(fOutPtr, "-------------------------------------------------\n");
            PrintResult2(cities_names, to, prev);
            fprintf(fOutPtr, "=================================================\n");
            fprintf(fOutPtr, "|\t  The cost from %s to %s  =  %d\t\n", cities_names[from], cities_names[to], dist[to]);
            fprintf(fOutPtr, "=================================================\n\n");
        }else{
            //On screen ----------------------------------------------------------------------
            printf("=================================================\n");
            printf("|\tThe cost from %s to %s  =   %d\t|\n", cities_names[from], cities_names[to], dist[to]);
            printf("=================================================\n");
            //In file ------------------------------------------------------------------------
            fprintf(fOutPtr, "=================================================\n");
            fprintf(fOutPtr, "|\t   The cost from %s to %s  =  %d\t\n", cities_names[from], cities_names[to], dist[to]);
            fprintf(fOutPtr, "=================================================\n");
        }
    }
    else {
        printf("\n\tThere is no way from %s to %s\n", cities_names[from], cities_names[to]);
        fprintf(fOutPtr, "There is no way from %s to %s\n", cities_names[from], cities_names[to]);
    }
    fclose(fOutPtr);
}

void DijkstraSourceOnly(edge graph[][numberOfCities], char cities_names[][100], int from){ //if only source then get all the possible destination and do dijkstra
    for(int i=0; i < numberOfCities; i++){
        Dijkstra(graph, cities_names, from, i);
    }
}

void menu(){ //this function is used to print the menu.
    printf("-------------------------------------------------Menu-----------------------------------------------------------\n");
    printf("\t..........................................................................................\n");
    printf("\t:\t1. Load Cities.\t\t\t\t\t\t\t\t\t :\n");
    printf("\t:\t2. Enter Source City.\t\t\t\t\t\t\t\t :\n");
    printf("\t:\t3. Enter destination city:  \t\t\t\t\t\t\t :\n");
    printf("\t:\t4. Exit.\t\t\t\t\t\t\t\t         :\n");
    printf("\t..........................................................................................\n\n");
    printf("What is your Choice?\n");
}

int main() {
    cities = fopen("Cities.txt", "r");
    if(cities == NULL) { //if file not found
        printf("file Cities.txt cannot be found");
        exit(1);
    }

    FILE *cities2 = fopen("Cities.txt", "r");
    if(cities2 == NULL) { //if file not found
        printf("file Cities.txt cannot be found");
        exit(1);
    }
    int num = countlines(cities2); //using the 2nd file to read num of lines
    char arr[num][100]; //to store all cities in it with repeat
    char line[200];
    int i = 0;

    while(fgets(line, 200, cities)) { //read the sources
        char city_name[100];

        char *token;
        token = strtok(line, " ");
        strcpy(city_name, trimwhitespace(token));
        strcpy(arr[i], city_name); //put them in the array
        i++;
    }

    int flag = 0;
    for(int i=0; i < num; i++){ //to get the number of unrepeated cities
        for(int j=0; j < num; j++){
            if(i == j){
                continue;
            }
            if(strcmp(arr[i], arr[j]) == 0){
                i = j;
            }
        }
        numberOfCities++;
    }

    int count=0;
    char cities_names[numberOfCities][100]; //array of cities unrepeated
    for(int i=0; i < num; i++){ //if not repeated add to array
        for(int j=0; j < num; j++){
            if(i == j){
                continue;
            }
            if(strcmp(arr[i], arr[j]) == 0){
                i = j;
            }
        }
        strcpy(cities_names[count], arr[i]);
        count++;
    }

    fclose(cities2);
    fclose(cities);

    edge graph[numberOfCities][numberOfCities]; //the 2D array for dijkstra.

    //--------------------------------------------------------------------------------------

    printf("\t\t\t\t         Welcome to my program\n");
    printf("\t       This program tells you the shortest path between cities. Check it out!\n\n");
    int selection = 0; //selection to know which thing from the menu it wants to do
    menu(); //printing menu on the screen for the user
    char city[100], source[100], dis[100];
    int index1 = -1, indexSource = -1, indexDis = -1;
    scanf("%d", &selection); //reading the selection

    while(1){
        switch (selection) {
            case 1:
                printf("Info Loaded Successfully!!\n\n");
                loadInfo(graph, cities_names);
                break;
            case 2:
                if(loaded == 1){
                    index1 = -1;
                    printf("\nPlease choose one of these cities:\n");
                    for(int i=0; i < numberOfCities; i++){
                        printf("%d : %s\n", i+1, cities_names[i]);
                    }
                    printf("====================\n");
                    printf("Please Enter The Source City To Get You All Shortest Paths:\n");
                    scanf("%s", city);

                    for(int i=0; i < numberOfCities; i++){
                        if(strcmp(cities_names[i], city) == 0){
                            index1 = i;
                            break;
                        }
                    }

                    while(index1 == -1){
                        printf("\nThis City Is Not Valid!!! Enter Another City Name:\n");
                        scanf("%s", city);
                        for(int i=0; i < numberOfCities; i++){
                            if(strcmp(cities_names[i], city) == 0){
                                index1 = i;
                                break;
                            }
                        }
                    }

                    FILE  *fOut;
                    fOut = fopen("shortest_path.txt", "w");
                    fclose(fOut);
                    DijkstraSourceOnly(graph, cities_names, index1);
                    printf("\n");
                }else{
                    printf("Please Load Info From File First!!\n\n");
                }

                break;
            case 3:
                if(loaded == 1){
                    indexDis = -1;
                    indexSource = -1;

                    printf("\nPlease choose one of these cities:\n");
                    for(int i=0; i < numberOfCities; i++){
                        printf("%d : %s\n", i+1, cities_names[i]);
                    }
                    printf("====================\n");
                    printf("Please Enter The Source City To Get The Shortest Path:\n");
                    scanf("%s", source);

                    for(int i=0; i < numberOfCities; i++){
                        if(strcmp(cities_names[i], source) == 0){
                            indexSource = i;
                            break;
                        }
                    }

                    while(indexSource == -1){
                        printf("\nThis Source City Is Not Valid!!! Enter Another City Name:\n");
                        scanf("%s", source);
                        for(int i=0; i < numberOfCities; i++){
                            if(strcmp(cities_names[i], source) == 0){
                                indexSource = i;
                                break;
                            }
                        }
                    }

                    printf("\nPlease Enter The Destination City To Get The Shortest Path:\n");
                    scanf("%s", dis);

                    for(int i=0; i < numberOfCities; i++){
                        if(strcmp(cities_names[i], dis) == 0){
                            indexDis = i;
                            break;
                        }
                    }

                    while(indexDis == -1){
                        printf("\nThis Destination City Is Not Valid!!! Enter Another City Name:\n");
                        scanf("%s", dis);
                        for(int i=0; i < numberOfCities; i++){
                            if(strcmp(cities_names[i],dis) == 0){
                                indexDis = i;
                                break;
                            }
                        }
                    }

                    FILE  *fOut2;
                    fOut2 = fopen("shortest_path.txt", "w");
                    fclose(fOut2);
                    Dijkstra(graph, cities_names, indexSource, indexDis);
                    printf("\n");
                }else{
                   printf("Please Load Info From File First!!\n\n");
                }

                break;
            case 4:
                printf("\nData is Saved to the file successfully\n");
                exit(0);
                break;
            default:
                printf("\nPlease enter number between 1-4.\n\n");
        }
        menu(); //putting the menu again on screen
        scanf("%d", &selection); //reading the selection for the while loop
    }
}