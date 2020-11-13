#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <linux/types.h>
#include <linux/spi/spidev.h>

#define MAXTIME 5
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static void pabort(const char *s)
{
        perror(s);
        abort();
}
static const char *device = "/dev/spidev0.1";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay;

static void transfer(int fd, int data)
{
        int ret;
        uint8_t tx[] = {data};
        struct spi_ioc_transfer tr = {
                .tx_buf = (unsigned long)tx;
                .len = ARRAY_SIZE(tx),
                .delay_usecs = delay,
                .speed_hz = speed,
                .bits_per_word = bits,
        };
        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1)
                pabort("can't send spi message");
}

void delay(int seconds)
{
    int microseconds= 1000000 * seconds;
    clock_t start_time = clock();
    while(clock()< start_time+ microseconds);
}


volatile int i=0;
int main(int argc, char* argv[])
{
    int count = 0,iter;
    volatile int t=0;
    FILE *triggerdir,*echodir,*triggerout,*echoin;

    triggerdir = fopen("/sys/class/gpio/gpio45/direction","w");
    fseek(triggerdir,0,SEEK_SET);
    fprintf(triggerdir,"out");
    fflush(triggerdir);

    echodir = fopen("/sys/class/gpio/gpio44/direction","w");
    fseek(echodir,0,SEEK_SET);
    fprintf(echodir,"in");
    fflush(echodir);

    while(1)
    {
        int ret = 0;
        int fd;

        parse_opts(argc, argv);

        fd = open(device, O_RDWR);
        if (fd < 0)
                pabort("can't open device");
        ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
        if (ret == -1)
                pabort("can't set spi mode");
        ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
        if (ret == -1)
                pabort("can't set bits per word");
        ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
        if (ret == -1)
                pabort("can't set max speed hz");

        count = 0;
        triggerout = fopen("/sys/class/gpio/gpio44/value","w");
        echoin = fopen("/sys/class/gpio/gpio45/value","r");
        
        fseek(triggerout,0,SEEK_SET);
        fseek(echoin,0,SEEK_SET);
        
        fprintf(triggerout,"1");
        usleep(10);
        fflush(triggerout);
        
        triggerout = fopen("/sys/class/gpio/gpio44/value","w");
        fprintf(triggerout,"0");
        usleep(2);
        fflush(triggerout);
        
        fscanf(echoin,"%d",&i);
        printf("vALUE OF i = %d\n", i);
        while(!i)
        {  
            printf("Obstacle = %d\n", i);
            fflush(echoin);
            echoin = fopen("/sys/class/gpio/gpio45/value","r");
            fscanf(echoin,"%d",&i);
            sleep(1);
            count++;
            if(count > MAXTIME)
                break;
        }
        while(i)
        {
            printf("obs = %d\n", i);
            fflush(echoin);
            echoin = fopen("/sys/class/gpio/gpio45/value","r");
            fscanf(echoin,"%d",&i);
            t++;
        }
        //fread(i,1,1,echoin);
        //printf("echoval = %d\n",i);
        //sleep(1);
        printf("Time = %d\n", t);
        fflush(echoin);
        printf("spi mode: %d\n", mode);
        printf("bits per word: %d\n", bits);
        printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
        transfer(fd, time);
        t = 0;
    }
    fclose(triggerdir);
    fclose(echodir);
    fclose(triggerout);
    fclose(echoin);
    close(fd); 
    return 0;
}
