#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

void delay(int seconds)
{
    int microseconds= 1000000 * seconds;
    clock_t start_time = clock();
    while(clock()< start_time+ microseconds);
}

volatile int pirval=0;
int main(void)
{
    int t=0;
    FILE *pirdir,*buzzerdir,*buzzerout,*pirin;

    buzzerdir = fopen("/sys/class/gpio/gpio67/direction","w");
    fseek(buzzerdir,0,SEEK_SET);
    fprintf(buzzerdir,"out");
    fflush(buzzerdir);
   
    buzzerdir = fopen("/sys/class/gpio/gpio67/active_low","w");
    fseek(buzzerdir,0,SEEK_SET);
    fprintf(buzzerdir,"1");
    fflush(buzzerdir);

    pirdir = fopen("/sys/class/gpio/gpio69/direction","w");
    fseek(pirdir,0,SEEK_SET);
    fprintf(pirdir,"in");
    fflush(pirdir);

    while(1)
    {
        buzzerout = fopen("/sys/class/gpio/gpio67/value","w");
        pirin = fopen("/sys/class/gpio/gpio69/value","r");
        fseek(buzzerout,0,SEEK_SET);
        fseek(pirin,0,SEEK_SET);
        fscanf(pirin, "%d", &pirval);
        printf("PIR VAL = %d\n", pirval);
        if(pirval)
        {
            fprintf(buzzerout, "1");
        }
        else
        {
            fprintf(buzzerout, "0");
        }
        fflush (pirin);
        fflush(buzzerout);
        delay(1);
    }
    fclose(buzzerdir);
    fclose(pirdir);
    fclose(buzzerout);
    fclose(pirin);
    return 0;
}
