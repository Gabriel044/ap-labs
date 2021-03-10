#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define REPORT_FILE "packages_report.txt"
#define HASHSIZE 101
#define PACKAGE_MANAGER "ALPM"

void analizeLog(char *logFile, char *report);
int compareStrings(char *s1, char *s2);
unsigned hash(char *s);

struct tnode {        
    struct package *node;                     
    struct tnode * left;   
    struct tnode * right;  
};

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

struct tnode *getNode(struct tnode *current, char *package_name);
int areEqual(struct package *package_node, char *package_name);
int comparePackage(struct package* package_1, char *package_name);
void writePackages(FILE *writer);
struct p_list *insert(char *name, struct package *current);
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
    printf("%s\n",argv[2]);
    int num;
    analizeLog(argv[2], REPORT_FILE);

    return 0;
}

void analizeLog(char *logFile, char *report) {
    int created = 0;
    printf("Generating Report from: [%s] log file\n", logFile);
    //struct tnode * root = NULL;
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
        /*  
        if(root != NULL){
            printf("root pre-starting: %s\n",root->node->name);
        }    */
        printf("%s\n",line);
        input_date = strtok(line, "[]\n");
        package_manager = strtok(NULL, " []\n");
        action = strtok(NULL, " []\n");
        input_name = strtok(NULL, " []\n");
        char date[strlen(input_date)];
        char name[strlen(input_name)];
        strcpy(date,input_date);
        strcpy(name,input_name);
        printf("date: %s, name: %s, action: %s\n", date, name, action);
        printf("%s\n",package_manager);        

        if(compareStrings(package_manager,PACKAGE_MANAGER)){
            struct package *current_package;
           
            printf("Aproved loop \n");
            /*
            if(root != NULL){
                printf("root starting: %s\n",root->node->name);
                printf("past package: %s\n",current_package->name);
            }*/
            //printf("Is installing ? %d\n",compareStrings(action, "installed"));
            //printf("Is updating ? %d\n",compareStrings(action, "update"));
            //struct tnode * tempNode = getNode(root, name);
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
                    created++;
                    printf("Installing...\n");
                }
            }else{
                current_package = current_element->node;
                if(compareStrings(action, "upgraded")){
                    printf("%s upgraded\n",name);
                    if(current_package->last_update == NULL){
                        current_package->last_update = malloc(sizeof(input_date));
                    }
                    strcpy(current_package->last_update,input_date);
                    current_package->updates++;   
                    upgraded_packages++;             
                }else if(compareStrings(action, "installed")){
                    printf("%s installed!\n",name);
                    //free((void *)current_package->install_date);
                    //current_package->install_date = malloc(sizeof(input_date)); 
                    strcpy(current_package->install_date,input_date);
                    current_installed++;
                    if(removed_packages > 0){
                        removed_packages--;
                    }
                }else if(compareStrings(action, "removed")){
                    printf("%s removed\n",name);
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
           
        }
        if(i++ == 1000) break;
        printf("packages created: %d\n",created);
    }
    fclose(fp);
    printf("Here at last\n");
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
    printf("we made it till here!\n");
    writePackages(fptr);
    fclose(fptr);
    printf("Installed packages : %d\n",installed_packages);
    printf("Removed packages : %d\n",removed_packages);
    printf("Upgraded packages : %d\n",upgraded_packages);
    printf("Current installed : %d\n",current_installed);
    printf("Report is generated at: [%s]\n", report);
}

struct tnode *getNode(struct tnode *current, char *package_name){
    printf("Inside recursion\n");
    if(current != NULL){
        printf("The current node has the package %s\n",current->node->name);
    }
    if(compareStrings(package_name,"gnupg")){
        printf("Checa esta iteracion!!!!!!!\n");
    }
    if(current == NULL){
        printf("It's null\n");
        /*
        current = (struct tnode *) malloc(sizeof(struct tnode));
        current->node = (struct package *) malloc(sizeof(struct package));
        current->left = current->right = NULL;
        //current->node->updates = 0; I thinks this should work now.
        installed_packages++;*/
        return current;
    }else if(areEqual(current->node, package_name)){ //When the variable is not a pointer, pass address with &.
        printf("already existant %s\n",package_name);
        printf("package name: %s\n",current->node->name);
        return current;
    }else if(comparePackage(current->node, package_name)){
        printf("Is greater than...\n");
        return getNode(current->left, package_name);
    }else{
        printf("Is less than...\n");
        return getNode(current->right, package_name);
    }
}

void writePackages(FILE *writer){
    /*
    if(current != NULL){
        writePackage(current->left, writer);
        fprintf(writer,"- Package Name        : %s\n",current->node->name);
        fprintf(writer,"  - Install date      : %s\n",current->node->install_date);
        fprintf(writer,"  - Last update date  : %s\n",
        current->node->last_update == NULL ? "-" : current->node->last_update);
        fprintf(writer,"  - How many updates  : %d\n",current->node->updates);
        fprintf(writer,"  - Removal date      : %s\n",
        current->node->removal_date == NULL ? "-" : current->node->removal_date);
        writePackage(current->right, writer);
    }*/
    struct p_list *current_element;
    struct package *current_package;
    for(int i = 0; i < HASHSIZE; i++){
        printf("We are inside the loop\n");
        if((current_element = Hashtable[i]) != NULL){
            while(current_element != NULL){
                printf("We are inside the second loop\n");
                printf("Error must likely here\n");
                current_package = current_element->node;
                printf("current is: %s\n", current_package->name);
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
        printf("%s anddddd %s\n",name, np->node->name);
        if (compareStrings(name, np->node->name) == 1){
            printf("We survived!!!\n");
            return np;
        }    
    }
    return NULL;        
}

struct p_list *insert(char *name, struct package *current){
    struct p_list *np;
    unsigned hashval;
    if ((np = search(name)) == NULL) {  
        np = (struct p_list *) malloc(sizeof(struct p_list));
        /*
        if (np == NULL ¦¦ (np->name = strdup(name)) == NULL){
            return NULL;
        }*/
        np->next = NULL;
        np->node = current;
        hashval = hash(name);
        //np->next = hashtab[hashval];
        Hashtable[hashval] = np;
    } else      /* already there */
        while (np->next != NULL){
            np = np->next;
        }
        np->next = (struct p_list *) malloc(sizeof(struct p_list));
        np->next->next = NULL;
        np->next->node = current;
    /*
    if ((np->defn = strdup(defn)) == NULL)
        return NULL;*/
    return np;
}

int areEqual(struct package *package_node, char *package_name){
    printf("%s and %s Equal? \n", package_node->name, package_name);
    printf("%s\n",package_name);
    return compareStrings(package_node->name,package_name);
}

int comparePackage(struct package* package_node, char *package_name){
    printf("Comparison of %s and %s\n",package_name, package_node->name);
    int sum_1 = 0;
    int sum_2 = 0;
     
    for(char *name_1 = package_node->name; *name_1 != 0; name_1++){
        sum_1 += (int) *name_1;
    }
    for(char *name_2 = package_name; *name_2 != 0; name_2++){
        sum_2 += (int) *name_2;
    }
    printf("Ended?\n");
    return (sum_1 > sum_2) ? 1 : 0;
}

int compareStrings(char *s1, char *s2){
    char *p = s1, *c = s2;
    for(; *p != 0 && *c != 0; p++, c++){
        printf("Still here with %c and %c\n", *p,*c);
        if(*p != *c){
            return 0;
        }
    }
    printf("this test %d \n",(*p == 0 && *c == 0)); 
    return (*p == 0 && *c == 0) ? 1 : 0;
}
//359
