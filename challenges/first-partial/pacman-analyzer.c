#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define REPORT_FILE "packages_report.txt"
#define HASHSIZE 101
#define PACKAGE_MANAGER "ALPM"

void analizeLog(char *logFile, char *report);
int compareStrings(char *s1, char *s2);
unsigned hash(char *s);

struct p_list{
    struct package *node;                     
    struct p_list * next;  
};

struct package{
    char *name;
    char * install_date;
    char * last_update;
    unsigned int updates;
    char * removal_date;
};

void writePackages(FILE *writer);
void insert(char *name, struct package *current);
struct p_list *search(char *name);

static struct p_list *Hashtable[HASHSIZE];

unsigned int installed_packages = 0;
unsigned int upgraded_packages = 0;
unsigned int removed_packages = 0;
unsigned int current_installed = 0;

int main(int argc, char **argv) {

    if (argc < 2) {
        printf("Usage:./pacman-analizer.o \n");
        return 1;
    }
    //printf("%s\n",argv[2]);
    int num;
    analizeLog(argv[2], REPORT_FILE);

    return 0;
}

void analizeLog(char *logFile, char *report) {
    printf("Generating Report from: [%s] log file\n", logFile);
    struct p_list *current_element;
    FILE * fp;
    char * line = NULL, *input_date, *input_name, *action, *package_manager;
    size_t len = 0;
    ssize_t read;
    fp = fopen(logFile, "r");
    if(fp == NULL){
        printf("Input file not found!\n");
        return ;
    }
    int i = 0;
    while ((read = getline(&line, &len, fp)) != -1){  
        //printf("%s\n",line);
        input_date = strtok(line, "[]\n");
        package_manager = strtok(NULL, " []\n");
       //printf("%s\n",package_manager);      

        if(compareStrings(package_manager,PACKAGE_MANAGER)){
            action = strtok(NULL, " []\n");
            input_name = strtok(NULL, " []\n");
            char date[strlen(input_date)];
            char name[strlen(input_name)];
            strcpy(date,input_date);
            strcpy(name,input_name);
            //printf("date: %s, name: %s, action: %s\n", date, name, action);
            struct package *current_package;
           
            //printf("Aproved loop \n");
            if((current_element = search(name)) == NULL){
                if(compareStrings(action,"installed")){
                    current_package = (struct package *) malloc(sizeof(struct package));
                    current_package->install_date = malloc(sizeof(input_date));
                    strcpy(current_package->install_date, date);
                    current_package->name = malloc(sizeof(input_name));
                    strcpy(current_package->name, name);
                    current_package->updates = 0;
                    current_package->last_update = NULL;
                    current_package->removal_date = NULL;
                    insert(name,current_package);
                    installed_packages++;
                    current_installed++;
                }
            }else{
                current_package = current_element->node;
                if(compareStrings(action, "upgraded")){
                    //printf("%s upgraded!!!!!!\n",name);
                    if(current_package->last_update == NULL){
                        current_package->last_update = malloc(sizeof(input_date));
                    }
                    strcpy(current_package->last_update,input_date);
                    current_package->updates++;   
                    upgraded_packages++;             
                }else if(compareStrings(action, "installed")){
                    //printf("%s installed!\n",name);
                    //free((void *)current_package->install_date);
                    //current_package->install_date = malloc(sizeof(input_date)); 
                    strcpy(current_package->install_date,input_date);
                    current_installed++;
                    if(removed_packages > 0){
                        removed_packages--;
                    }
                }else if(compareStrings(action, "removed")){
                    //printf("%s removed\n",name);
                    if(current_package->removal_date == NULL){
                        current_package->removal_date = malloc(sizeof(input_date));
                    }
                    strcpy(current_package->removal_date,input_date);
                    if(current_installed > 0){
                        current_installed--;
                    }
                    removed_packages++;
                }
            }
            if(i++ ==700) break;
        }
    }
    fclose(fp);
    if (line)
        free(line);
    FILE *fptr;
    fptr = fopen(report,"w");

    if(fptr == NULL){
        printf("Error!");   
        exit(1);             
    }

    fprintf(fptr,"Installed packages : %d\n",installed_packages);
    fprintf(fptr,"Removed packages : %d\n",removed_packages);
    fprintf(fptr,"Upgraded packages : %d\n",upgraded_packages);
    fprintf(fptr,"Current installed : %d\n",current_installed);
    fprintf(fptr,"\nList of packages\n");
    fprintf(fptr,"----------------\n");
    writePackages(fptr);
    fclose(fptr);
   // printf("Installed packages : %d\n",installed_packages);
   // printf("Removed packages : %d\n",removed_packages);
   // printf("Upgraded packages : %d\n",upgraded_packages);
   // printf("Current installed : %d\n",current_installed);
    printf("Report is generated at: [%s]\n", report);
}

void writePackages(FILE *writer){
    struct p_list *current_element;
    struct package *current_package;
    for(int i = 0; i < HASHSIZE; i++){
        if((current_element = Hashtable[i]) != NULL){
            while(current_element != NULL){
                current_package = current_element->node;
                fprintf(writer,"- Package Name        : %s\n",current_package->name);
                fprintf(writer,"  - Install date      : %s\n",current_package->install_date);
                fprintf(writer,"  - Last update date  : %s\n",
                current_package->last_update == NULL ? "-" : current_package->last_update);
                fprintf(writer,"  - How many updates  : %d\n",current_package->updates);
                fprintf(writer,"  - Removal date      : %s\n",
                current_package->removal_date == NULL ? "-" : current_package->removal_date);
                current_element = current_element->next;
            }
        }
    }
}

unsigned hash(char *s){
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    return hashval % HASHSIZE;
}

struct p_list *search(char *name){
    struct p_list *np;
    for (np = Hashtable[hash(name)]; np != NULL; np = np->next){
        //printf("%s anddddd %s\n",name, np->node->name);
        if (compareStrings(name, np->node->name) == 1){
            //printf("We survived!!!\n");
            return np;
        }    
    }
    return NULL;        
}

void insert(char *name, struct package *current){
    struct p_list *np;
    unsigned hashval;
    if ((np = search(name)) == NULL) {  
        np = (struct p_list *) malloc(sizeof(struct p_list));
       
        np->next = NULL;
        np->node = current;
        hashval = hash(name);
        Hashtable[hashval] = np;
    } else {    
        while (np->next != NULL){
            np = np->next;
        }
        np->next = (struct p_list *) malloc(sizeof(struct p_list));
        np->next->next = NULL;
        np->next->node = current;
    }
}

int compareStrings(char *s1, char *s2){
    char *p = s1, *c = s2;
    
    for(; *p != 0 && *c != 0; p++, c++){
        if(*p != *c){
            return 0;
        }
    }
    return (*p == 0 && *c == 0) ? 1 : 0;
}