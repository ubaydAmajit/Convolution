#include <stdio.h>
#include <stdlib.h>
#include<time.h>

void calculate_convolution(int** data, int kernal[5][5], int** rlt, int x, int y){
    int sum=0, xBound, yBound;
    for(int col = 0; col < 5; col++){ // loop  interchange - row major
        for(int row = 0; row < 5; row++){//row is my inner loop, demonstrates spatial locality
            xBound = (x+row)-2, yBound = (y+col)-2;
            if(((yBound > -1 && yBound < 1024) && (xBound > -1 && xBound < 1024))){
                sum += data[yBound][xBound] * kernal[4-col][4-row]; //curr data val* curr kernal val in sum. keep accessing ybound and xbound
            }//temp loca xbound ybound col row <= fast
        }
    }
    rlt[y][x] = sum;
}

void saturate_scaling(int** rlt, int x, int y){//spatial locality - using index of col and row 
    int step_2 = rlt[x][y]/16;
    rlt[x][y] = (step_2 > 16) ? 16 : (step_2 < -16) ? -16 : step_2;
}

void copy_matrix(int** data, int** rlt, int x, int y){
    data[x][y] = rlt[x][y];//
}

int main(int argc, char *argv[]) {
    FILE *file1, *file2, *file3;
    int i = 0;
    int filter[5][5];
    int** data;
    int** rlt;
    int j, k, l, m;
    int val;
    int iter;

    if (argc < 5) {
    	printf("Usage: ./convolution2 data1.txt filter1.txt temp111 1\n");
    	return 1;
    }
    file1 = fopen(argv[1], "r");
    file2 = fopen(argv[2], "r");
    if ((file1 == NULL) || (file2 == NULL)) {
    	printf("Error: Files cannot open\n");
    	return 1;
    }
    iter = atoi(argv[4]);
    if (iter == 0) {
    	printf("Error: Invalid iteration\n");
    	return 1;
    } 
    file3 = fopen(argv[3], "w");
    data = (int**) malloc(sizeof(int*)*1024);
    rlt = (int**) malloc(sizeof(int*)*1024);
    for (i = 0; i < 1024; i++) {
        data[i] = (int*) malloc(sizeof(int)*1024);
        rlt[i] = (int*) malloc(sizeof(int)*1024);
    }

    i = 0;
    do {
        j = 0;
        do {
            if (j == 1023) {
                fscanf(file1, "%d\n", &val);
                data[i][j] = val;
//                printf("%d\n", data[i][j]);
            }
            else {
                fscanf(file1, "%d,", &val);
                data[i][j] = val;
//                printf("%d ", data[i][j]);
            }
            j = j + 1;
        } while (j < 1024);
        i = i + 1;
    } while (i < 1024);

    i = 0;
    do {
        int j = 0;
        do {
            if (j == 4) {
                fscanf(file2, "%d\n", &val);
                filter[i][j] = val;
//                printf("%d\n", filter[i][j]);
            }
            else {
                fscanf(file2, "%d,", &val);
                filter[i][j] = val;
//               printf("%d ", filter[i][j]);
            }
            j = j + 1;
        } while (j < 5);
        i = i + 1;
    } while (i < 5);


// Your code is here _____________________________________________
    //perform convolution
    for(int i = 0; i < iter; i++){
        for (int y = 0; y < 1024; y++) {
            for (int x = 0; x < 1024; x++) {
                calculate_convolution(data, filter, rlt, x, y);
            }
        }


        for(int x = 0; x < 1024; x++){
            for(int y = 0; y < 1024; y++){
                saturate_scaling(rlt, x, y);
                copy_matrix(data, rlt, x, y);
            }
        }
    }
//_________________________________________________________________

    for (i = 0; i < 1024; i++) {
        for (j = 0; j < 1024; j++) {
            fprintf(file3, "%d ", data[i][j]);
        }
        fprintf(file3, "\n");
    }

    for (i = 0; i < 1024; i++) {
        free(data[i]);
        free(rlt[i]);
    }
    free(data);
	free(rlt);
    fclose(file1);
    fclose(file2);
    fclose(file3);
    return 0;
}
