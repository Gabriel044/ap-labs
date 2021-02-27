#include <stdio.h>
#include <stdlib.h>

#define OUT 0
#define IN 1

char * reverse(int length, char word[50]){
    char temp;
    for(int k = 0; k < length/2; k++){
        if(k != length - (1 + k)){
            temp = word[k];
            word[k] = word[length - (1 + k)];
            word[length - (1 + k)] = temp;
        }
    }
    return word;
}

int main(int argc, char **argv){
    // Place your magic here
    //getchar() -> Reads a character from stdin
    int i, wordLength ;
    i = 0;
    wordLength = 0;
    char *input = malloc(50*sizeof(char));
    printf("%s",input);
    char c;
    
    while((c = getchar()) != EOF){
        
        if(c == '\n' || c == ' ' || c == '\t'){
           printf("%s ",reverse(wordLength,input));
            wordLength = 0;
            i = 0;
            
        }else {
            input[i] = c;
            i ++;
            wordLength ++;
        }
    }
    printf("\n");
    return 0;
}
