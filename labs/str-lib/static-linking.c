#include <stdio.h>
#include <string.h>

int mystrlen(char *str);
char *mystradd(char *origin, char *addition);
int mystrfind(char *origin, char *substr);
int compareStrings(char *s1, char *s2);

int main(int argc, char **argv) {

    if(argc < 4){
        printf("You need at least three arguments to run this program.\n");
        return 0;
    }
    char *first_word = argv[2];
    char *second_word = argv[3];

    if(compareStrings(argv[1],"-add")){
        printf("Initial length:   %d\n",mystrlen(first_word));
        char *merged_word = mystradd(first_word,second_word);
        printf("New String:       %s\n",merged_word);
        printf("New length:       %d\n",mystrlen(merged_word));
    }else if(compareStrings(argv[1],"-find")){
        int position = mystrfind(first_word,second_word);
        if(position < 0){
            printf("The substring was not found in the main stirng,\n");
        }else{
            printf("['%s'] string was found at [%d] position\n",second_word,position);
        }
    }else{
        printf("Request not recognized\n");
    }

    return 0;
}

int compareStrings(char *s1, char *s2){
    char *base = s1;
    char *compare = s2;
    for (; *base != 0 && *compare != 0; base++, compare++){
        if(*base != *compare) return 0;
    }
    return (*base == 0 && *compare == 0) ? 1 : 0;
}
