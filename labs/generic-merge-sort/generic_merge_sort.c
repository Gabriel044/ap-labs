#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINES 100

int numcompare(char *number1, char *number2);
void mergeSort(void *elements[], int left, int right, int (*comp) (void *, void*));
void merge(void *elements[], int first, int last, int mid, int (*comp) (void *, void*));


void mergeSort(void *elements[], int first, int last, int (*comp) (void *, void*)){
    int num_elements = last - first;
    if(num_elements >= 1){            
        
        int mid = (first + last) / 2;
        mergeSort(elements, first, mid, comp);
        mergeSort(elements, mid +1, last, comp);
        merge(elements, first, last, mid, comp);
    }
}

void merge(void *elements[], int first, int last, int mid, int (*comp) (void *, void*)){
    
    int left_array = first,
        right_array = mid +1,
        sorted_pos = 0, 
        n = last-first+1;
    void *sorted[n];
    while (left_array <= mid && right_array <= last){
        printf("Survived\n");
        if((*comp)(elements[left_array],elements[right_array]) < 0){
            sorted[sorted_pos] = (void *) malloc(sizeof(elements[left_array]));
            strcpy(sorted[sorted_pos++],elements[left_array++]);
        }else{
            sorted[sorted_pos] = (void *) malloc(sizeof(elements[right_array]));
            strcpy(sorted[sorted_pos++],elements[right_array++]);
        }
        printf("Suradavived\n");
    }
    
    while (left_array <= mid){
        sorted[sorted_pos] = (void *) malloc(sizeof(elements[left_array]));
        strcpy(sorted[sorted_pos++],elements[left_array++]);
    }
    printf("Survivddded\n");
    while (right_array <= last){
        //sorted[sorted_pos] = (void *) malloc(sizeof(elements[right_array]));
        //strcpy(sorted[sorted_pos++],elements[right_array++]);
        sorted[sorted_pos] = elements[right_array];
        printf("%s\n",sorted[sorted_pos]);
        right_array++;
        sorted_pos++;
    }
    printf("Survivddded2\n");
    for(int i = 0; i < n; i++){
        free(elements[first]);
        //elements[first] = (void *) malloc(sizeof(sorted[i]));
        //strcpy(elements[first++],sorted[i]);
        elements[first++] = sorted[i];
    }
    
}



int numcompare(char *number1, char *number2){
    return (atoi(number1) > atoi(number2)) ? 1 : -1;
}

int main(int argc, char **argv){
    FILE *input, 
         *output;
    int numeric = 0, 
        pos = 0;
    size_t len;
    ssize_t nlines;
   char *lines[MAXLINES], 
        *line;
    if(argc > 1 && strcmp(argv[1],"-n") == 0){
        numeric = 1;
        input = fopen(argv[2],"r");
        output = fopen(argv[4],"w");
    }else{
        input = fopen(argv[1],"r");
        output = fopen("sorted_strings.txt","w");
    }
    
    while ((nlines = getline(&line,&len,input)) != -1){
        
        lines[pos] = (char *) malloc(nlines);
        strcpy(lines[pos++],strtok(line,"\n")); 
    }
    
    mergeSort((void **) lines, 0, pos-1, 
    (int (*) (void*, void*))(numeric ? numcompare : strcmp));
    for(int i = 0; i < pos; i++){
        fprintf(output, "%s\n",lines[i]);
    }
    
    fclose(input);
    fclose(output);
    return 0;
}
