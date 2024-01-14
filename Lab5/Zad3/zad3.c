#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

double f(double x){
    return 4.0/(x*x+1);
}

double rectangle_area(double width, double a, double b){
    double area=0.0;
    while (a+width<b){
        area+=width*f(a);
        a+=width;
    }
    area+=(b-a)*f(a);
    return area;
}


int main(int argc, char* argv[]){
    if (argc!=3){
        printf("Za mało danych wejściowych!\n");
        return 0;
    }
    double width= strtod(argv[1],NULL);
    int n_processes=strtol(argv[2],NULL,10);

    struct timeval start, end;

    gettimeofday(&start,NULL);
    double h=1.0/n_processes;

    double area_sum=0;

    char* path="fifo_queue";
    mkfifo("fifo_queue", 0600);
    int pipe;
    for (int i=0;i<n_processes;i++){
        pid_t pid=fork();
        if (pid==0){
            pipe= open("fifo_queue",O_WRONLY);
            double area= rectangle_area(width,0+h*i,0+h*(i+1));
            write(pipe,&area,sizeof(area));
            close(pipe);
            exit(0);
        }
    }

    pipe= open(path,O_RDONLY);
    for (int i=0;i<n_processes;i++){
        double area;
        read(pipe,&area, sizeof(area));
        area_sum+=area;
    }

    close(pipe);
    unlink(path);

    gettimeofday(&end,NULL);
    double time=(double)end.tv_sec-(double)start.tv_sec;
    time+=(double)(end.tv_usec-start.tv_usec)/1000000;

    printf("pole: %.16f\n",area_sum);
    printf("czas: %f\n",time);

    FILE* report=fopen("raport.txt","a");
    if (report!=NULL){
        fprintf(report,"Wynik obliczeń: %.16f, czas trwania: %f, dokładność obliczeń: %.16f, ilość procesów potomnych: %d\n",area_sum,time,width,n_processes);
        fclose(report);
    }

    return 0;
}