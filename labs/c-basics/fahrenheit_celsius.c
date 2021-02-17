#include <stdio.h>
#include <stdlib.h>

#define   LOWER  0       /* lower limit of table */
#define   UPPER  300     /* upper limit */
#define   STEP   20      /* step size */

/* print Fahrenheit-Celsius table */

int main(int argc, char **argv)
{
    int fahr = atoi(argv[1]);
    
    /*
    for (fahr = LOWER; fahr <= UPPER; fahr = fahr + STEP)
    */
    if(argc == 4){
        int upper = atoi(argv[2]);
        int step = atoi(argv[3]);
        for (fahr ; fahr <= upper; fahr = fahr + step)
        printf("Fahrenheit: %3d, Celcius: %6.1f\n", fahr, (5.0/9.0)*(fahr-32));
    }else if(argc == 2){
        printf("Fahrenheit: %3d, Celcius: %6.1f\n", fahr, (5.0/9.0)*(fahr-32));
    }else{
        printf("Error you sent the wrong number of arguments \n");
        return -1;
    }
    
    return 0;
}