#define _GNU_SOURCE
#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>

#include "logger.h"

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
int FD;
char *MAIN_DIR;
char *sub_dirs[50];

void getRelativePath(char *absolute_path, char *relative){
    char *pointer_abs = absolute_path;
    for(char *pointer_main =  MAIN_DIR; *pointer_main != '\0'; pointer_abs++, pointer_main++);
    pointer_abs++;
    int i = 0;
    for(; *pointer_abs != '\0'; pointer_abs++){
        relative[i++] = *pointer_abs;
    }
    relative[i] = '\0';
}

int getTreeLevel(char *relative){
    int levels = 0;
    for(char *p = relative; *p != '\0'; p++){
        if(*p == '/'){
            levels++;
        }
    }
    return levels;
}

static void dipslayEvent(struct inotify_event *event){
    if(!(event->mask & IN_DELETE_SELF) && !(event->mask & IN_IGNORED)){
        char log[250]; 
    
        ((event->mask & IN_MOVED_TO) && (event->cookie != 0)) ? strcpy(log,"") : strcpy(log,"- [");

        if(!(event->mask & IN_MOVED_TO) || (event->cookie == 0)){
            (event->mask & IN_ISDIR) ? strcat(log, "Directory") : strcat(log, "File");
            strcat(log, " - ");
        }
        if(event->mask & IN_CREATE) strcat(log, "Create");
        if(event->mask & IN_DELETE) strcat(log, "Removal");
        if((event->mask & IN_MOVED_FROM) && (event->cookie != 0)) strcat(log, "Rename");

        if(!(event->mask & IN_MOVED_TO) || (event->cookie == 0)) strcat(log, "] - ");
        if((event->wd > 1) && !(event->mask & IN_DELETE_SELF)) strcat(log,sub_dirs[event->wd-1]);
        strcat(log,event->name);
        ((event->mask & IN_MOVED_FROM) && (event->cookie != 0)) ? strcat(log," -> ") : strcat(log,"\n");
        infof(log);
        if(event->mask & IN_ISDIR){
            if(event->mask & IN_DELETE_SELF) inotify_rm_watch(FD,event->wd);
            if(event->mask & IN_CREATE){
                char *path;
                if(event->wd > 1){
                    path = malloc(strlen(MAIN_DIR)+strlen(sub_dirs[event->wd-1])+event->len+2);
                    strcpy(path, MAIN_DIR);
                    strcat(path,"/");
                    strcat(path, sub_dirs[event->wd-1]);
                }else{
                    path = malloc(strlen(MAIN_DIR)+event->len+2);
                    strcpy(path, MAIN_DIR);
                    strcat(path,"/");
                }
                
                strcat(path,event->name);
                char relative[200];
                getRelativePath(path,relative);
                if(getTreeLevel(relative) < 2){
                    int wd = inotify_add_watch(FD,path,IN_CREATE|IN_DELETE|IN_DELETE_SELF|IN_MOVED_TO|IN_MOVED_FROM);
                    if(wd == -1){
                        errorf("inotify_add_watch\n");
                    }
                    sub_dirs[wd-1] = malloc(strlen(relative)+2);
                    strcpy(sub_dirs[wd-1],relative);
                    strcat(sub_dirs[wd-1],"/");
                }
                free(path);
            }
        }
    }
}

static int
display_info(const char *fpath, const struct stat *sb,
             int tflag, struct FTW *ftwbuf)
{ 
    if((tflag & FTW_D) && (ftwbuf->level < 3)){
        int wd = inotify_add_watch(FD,fpath,IN_CREATE|IN_DELETE|IN_DELETE_SELF|IN_MOVED_TO|IN_MOVED_FROM);
        if(wd == -1){
            errorf("inotify_add_watch\n");
            return (ftwbuf->level > 1) ? FTW_SKIP_SUBTREE: FTW_CONTINUE;
        }
        char relative[200];
        getRelativePath((char *)fpath,relative);
        if(*relative != '\0'){
            sub_dirs[wd-1] = malloc(strlen(relative)+2);
            strcpy(sub_dirs[wd-1],relative);
            strcat(sub_dirs[wd-1],"/");
        };
    }
    return (ftwbuf->level > 1) ? FTW_SKIP_SUBTREE: FTW_CONTINUE;
}

int main(int argc, char **argv){
    initLogger("stdout");
    if(argc < 2 || strcmp(argv[1],"--help") == 0){
        errorf("You need to run the program with the path to the desired directory\n");
        return 0;
    }

    MAIN_DIR = argv[1];  
    char buf[BUF_LEN] __attribute__ ((aligned(8)));
    ssize_t numRead;
    struct inotify_event *event;
    infof("Starting File/Directory Monitor on %s\n",MAIN_DIR);
    infof("-----------------------------------------------------\n");
    FD = inotify_init();
    if(FD == -1){
        errorf("inotify_init\n");
        return 0;
    }
    if(nftw(MAIN_DIR,display_info,20,FTW_ACTIONRETVAL) == -1){
        errorf("Error in nftw\n");
        return 0;
    }
    for(;;){
        numRead = read(FD, buf, BUF_LEN);
        if(numRead == 0) panicf("read() from inotify fd returned 0!\n");
        if(numRead == -1) errorf("read\n");
        for(char *p = buf; p < buf + numRead; ){
            event = (struct inotify_event *)p;
            dipslayEvent(event);
            p += sizeof(struct inotify_event) + event->len;
        }
    }
    return 0;
}