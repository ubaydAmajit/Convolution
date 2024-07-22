#include <stdio.h>
#include <stdlib.h>
#include<time.h>

struct matrix {
    int o_val;
    int n_val;
};
typedef struct matrix Matrix;


int calculate_convolution(Matrix** data, int kernal[5][5], int x, int y) {
        int sum = 0, xBound, yBound;
        //kernal iter row, col
        for(int col = 0; col < 5; col++){
            for(int row = 0; row < 5; row++){
                xBound = (x+row)-2, yBound = (y+col)-2;
                if((yBound > -1 && yBound < 1024) && (xBound > -1 && xBound < 1024)){ //inbound check
                    sum += data[yBound][xBound].o_val * kernal[4-col][4-row]; //curr data val* curr kernal val in sum.
                }  
            }
        }
        return sum;
    }

//step 1 is convoluted and step 2 is xscaled down to range(-16,16) and step1/16 if pass
int saturate_scaling(int step_1){
    int step_2 = step_1/16;
    return (step_2 > 16) ? 16 : (step_2 < -16) ? -16 : step_2;
}

//new vals in n_val copied back to orig pixel vals o_val
void copy_matrix(Matrix** data){
    for (int i = 0; i < 1024; i++) {
        for (int j = 0; j < 1024; j++) {data[i][j].o_val = data[i][j].n_val;}//merge array
    }
}


int main(int argc, char *argv[]) {
    FILE *file1, *file2, *file3;
    int i = 0;
    int filter[5][5];
    Matrix** data;
    int j, k, l, m;
    int val;
    int iter;

    if (argc < 5) {
    	printf("Usage: ./convolution1 data1.txt filter1.txt temp111 1\n");
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
    data = (Matrix**) malloc(sizeof(Matrix*)*1024);
    for (i = 0; i < 1024; i++) {
        data[i] = (Matrix*) malloc(sizeof(Matrix)*1024);
    }


    i = 0;
    do {
        j = 0;
        do {
            if (j == 1023) {
                fscanf(file1, "%d\n", &val);
                data[i][j].o_val = val;
//                printf("%d\n", data[i][j]);
            }
            else {
                fscanf(file1, "%d,", &val);
                data[i][j].o_val = val;
//                printf("%d ", data[i][j]);
            }
            j = j + 1;
        } while (j < 1024);
        i = i + 1;
    } while (i < 1024)
    ;

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




// Your code is here __________________________________________________________
    //perform convolution
    for (int x = 0; x < iter; x++) {
        //iter 2d data matrix for each pixel
        for (int k = 0; k < 1024; k++) {
            for (int i = 0; i < 1024; i++) {
                int convolution = calculate_convolution(data, filter, i, k);
                data[k][i].n_val = saturate_scaling(convolution);
            }
        }

        //copy n_val into o_val for next iter
        copy_matrix(data);
    }

    //___________________________________________________________________________

    for (i = 0; i < 1024; i++) {
        for (j = 0; j < 1024; j++) {
            fprintf(file3, "%d ", data[i][j].n_val);
        }
        fprintf(file3, "\n");
    }

    for (i = 0; i < 1024; i++) {
        free(data[i]);
    }
    free(data);
    fclose(file1);
    fclose(file2);
    fclose(file3);
    return 0;
}