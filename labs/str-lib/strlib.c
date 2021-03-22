#include <stdlib.h>

int mystrlen(char *str){
    int lenght = 0;
    for(char *c = str; *c != 0; c++){
        lenght++;
    }
    return lenght;
}

char *mystradd(char *origin, char *addition){
    int pos = 0, new_length = mystrlen(origin)+mystrlen(addition)+1;
    char *new_string = (char* ) malloc(new_length);
    
    for(char *c = origin; *c != 0; c++){
        new_string[pos++] = *c;
    }
    for(char *c = addition; *c != 0; c++){
        new_string[pos++] = *c;
    }
    new_string[pos] = 0; 
    return new_string;
}

int mystrfind(char *origin, char *substr){
    char *current = substr;
    int sub_length = mystrlen(substr),
        counter = 0, 
        current_position = 0,
        starting_position = -1;
    for(char *c = origin; *c != 0; c++){
        if(*c == *current++){
            counter++;
            if(starting_position < 0) starting_position = current_position;
            if(counter == sub_length) return starting_position;
        }else{
            counter == 0;
            starting_position = -1;
            current = &substr[0];
        }
        current_position ++;
    }
    return -1;
}
