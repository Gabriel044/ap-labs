#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* month_day function's prototype*/
void month_day(int yearday, int *pmonth, int *pday);

struct Month {
    char *name;
    int number;
};

const struct Month months[] = {{"Jan",1},{"Feb",2},{"Mar",3},{"Apr",4},
                                {"May",5},{"Jun",6},{"Jul",7},{"Aug",8},
                                {"Sep",9},{"Oct",10},{"Nov",11},{"Dec",12}};

int main(int argc, char **args) {
    int year = atoi(args[1]);
    int day = atoi(args[2]);
    if(day < 1 || day > 365){
        printf("There is not that many days in a year\n");
        return 0;
    }
    int month; 
    int monthday;
    month_day(day, &month, &monthday);
    printf("%s %d, %d\n", months[month].name, monthday, year);
    return 0;
}

void month_day(int yearday, int *pmonth, int *pday){
    *pmonth = floor(yearday/31);
    *pday = yearday%30 + 1;
}
