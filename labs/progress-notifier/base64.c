/*This code contains implementations for the encoding and decoding algorithms from WIKIBOOKS:
https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64#C*/

#include <stdio.h>
#include "logger.h"
#include <inttypes.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#define WHITESPACE 64
#define EQUALS     65
#define INVALID    66
#define BUFFER_SIZE 60000000

int stage; //Reading (0)/Writing (2)/Encoding (1)/Decoding (1)
char *mode;
float total, current;

static const unsigned char d[] = {
    66,66,66,66,66,66,66,66,66,66,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,62,66,66,66,63,52,53,
    54,55,56,57,58,59,60,61,66,66,66,65,66,66,66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,66,66,66,66,66,66,26,27,28,
    29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66
};


int base64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize) {
   const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   const uint8_t *data = (const uint8_t *)data_buf;
   size_t resultIndex = 0;
   size_t x;
   uint32_t n = 0;
   int padCount = dataLength % 3;
   uint8_t n0, n1, n2, n3;

   /* increment over the length of the string, three characters at a time */
   for (x = 0, current = 0; x < dataLength; x += 3, current += 3) 
   {
      /* these three 8-bit (ASCII) characters become one 24-bit number */
      n = ((uint32_t)data[x]) << 16; //parenthesis needed, compiler depending on flags can do the shifting before conversion to uint32_t, resulting to 0
      
      if((x+1) < dataLength)
         n += ((uint32_t)data[x+1]) << 8;//parenthesis needed, compiler depending on flags can do the shifting before conversion to uint32_t, resulting to 0
      
      if((x+2) < dataLength)
         n += data[x+2];

      /* this 24-bit number gets separated into four 6-bit numbers */
      n0 = (uint8_t)(n >> 18) & 63;
      n1 = (uint8_t)(n >> 12) & 63;
      n2 = (uint8_t)(n >> 6) & 63;
      n3 = (uint8_t)n & 63;
            
      if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
      result[resultIndex++] = base64chars[n0];
      if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
      result[resultIndex++] = base64chars[n1];

      if((x+1) < dataLength)
      {
         if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
         result[resultIndex++] = base64chars[n2];
      }

      if((x+2) < dataLength)
      {
         if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
         result[resultIndex++] = base64chars[n3];
      }
   }
   current = total - 1;
   if (padCount > 0) 
   { 
      for (; padCount < 3; padCount++) 
      { 
         if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
         result[resultIndex++] = '=';
      } 
   }
   if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
   result[resultIndex] = 0;
   return 0;   /* indicate success */
}


int base64decode (char *in, size_t inLen, unsigned char *out, size_t *outLen) { 
    char *end = in + inLen;
    char iter = 0;
    uint32_t buf = 0;
    size_t len = 0;
    current = 0;
    while (in < end) {
        unsigned char c = d[*in++];
        
        switch (c) {
        case WHITESPACE: continue;   /* skip whitespace */
        case INVALID:    return 1;   /* invalid input, return error */
        case EQUALS:                 /* pad character, end of data */
            in = end;
            continue;
        default:
            buf = buf << 6 | c;
            iter++; // increment the number of iteration
            /* If the buffer is full, split it into bytes */
            if (iter == 4) {
                if ((len += 3) > *outLen) return 1; /* buffer overflow */
                *(out++) = (buf >> 16) & 255;
                *(out++) = (buf >> 8) & 255;
                *(out++) = buf & 255;
                buf = 0; iter = 0;

            }   
        }
        current ++;
    }
   
    if (iter == 3) {
        if ((len += 2) > *outLen) return 1; /* buffer overflow */
        *(out++) = (buf >> 10) & 255;
        *(out++) = (buf >> 2) & 255;
    }
    else if (iter == 2) {
        if (++len > *outLen) return 1; /* buffer overflow */
        *(out++) = (buf >> 4) & 255;
    }

    *outLen = len; /* modify to reflect the actual output size */
    return 0;
}

void sigHandler(int sig){
    switch (stage)
    {
    case 0:
        infof("Reading file...\n");
        break;
    case 1:
        strcmp(mode,"encode") == 0 ? infof("Encoding progress: %f%\n",((current +1) / total)*100) : 
         infof("Decoding progress: %f%\n",((current +1) / total)*100);
        break;
    case 2:
        infof("Creating new file...\n");
        break;
    
    default:
        errorf("There has been an error.\n");
        break;
    }
    
}

char * read(char *reading_file){
    FILE *fptr;
    if ((fptr = fopen(reading_file,"r")) == NULL){
       errorf("There has been a problem opening the file, double check it is correct.\n");
       exit(1);
   }
   fseek(fptr, 0, SEEK_END);
   long fsize = ftell(fptr);
   fseek(fptr, 0, SEEK_SET);  /* same as rewind(f); */
   char * file = malloc(fsize + 1);
   fread(file, 1, fsize, fptr);
   fclose(fptr);
   return file;
}

void write(char *buffer){
    char *output_file = strcmp(mode,"encode") == 0 ? "encoded.txt" : "decoded.txt";
    FILE *fptr = fopen(output_file,"w");
    fprintf(fptr,buffer);
    fclose(fptr);
}


int main(int argc, char *argv[]){
    if(argc < 3){
        errorf("Not enough paramters\n");
        return 0;
    }
    initLogger("stdout");
    char *inpute_mode = argv[1];
    char *input_file = argv[2];
    if(strcmp(inpute_mode,"--encode") == 0){
        mode = "encode";
    }else if(strcmp(inpute_mode, "--decode") == 0){
        mode = "decode";
    }else{
        errorf("This command is not recognized.\n");
        return 0;
    }
    if(signal(SIGINT, sigHandler) == SIG_ERR) errorf("There was an error with the SIGINT handler.\n");
    if(signal(SIGUSR1, sigHandler) ==   SIG_ERR) errorf("There was an error with the SIGUSR1 handler.\n");
    stage = 0;
    char *file = read(input_file);
    stage = 1;
    size_t output_size = BUFFER_SIZE;
    char *transformed_file = calloc(BUFFER_SIZE,sizeof(char));
    total = strlen(file);
    strcmp(mode, "encode") == 0 ? base64encode(file, strlen(file), transformed_file, output_size) :
    base64decode(file,strlen(file),transformed_file,&output_size);
    stage = 2;
    write(transformed_file);
    return 0;
}
