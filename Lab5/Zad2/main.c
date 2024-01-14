#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

double f(double x) {
    return 4.0 / (x * x + 1.0);
}

double integrate(double a, double b, double dx) {
    double sum = 0.0;
    for (double x = a; x < b; x += dx) {
        sum += f(x + dx/2.0) * dx;
    }
    return sum;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Użycie: %s szerokość dx liczba_procesów\n", argv[0]);
        return 1;
    }
    double dx = atof(argv[1]);
    int num_procs = atoi(argv[2]);

    int fd[num_procs][2];
    pid_t pid[num_procs];
    double a = 0.0, b = 1.0;
    double h = (b - a) / num_procs;

    clock_t start = clock();

    for (int i = 0; i < num_procs; i++) {
        if (pipe(fd[i]) == -1) {
            perror("pipe");
            return 1;
        }
        pid[i] = fork();
        if (pid[i] == -1) {
            perror("fork");
            return 1;
        } else if (pid[i] == 0) { // proces potomny
            close(fd[i][0]); // zamknięcie nieużywanego końca potoku
            double a_i = a + i * h;
            double b_i = a + (i + 1) * h;
            double result = integrate(a_i, b_i, dx);
            write(fd[i][1], &result, sizeof(result));
            close(fd[i][1]); // zamknięcie końca zapisującego
            return 0;
        } else { // proces macierzysty
            close(fd[i][1]); // zamknięcie nieużywanego końca potoku
        }
    }

    double total_result = 0.0;
    for (int i = 0; i < num_procs; i++) {
        double result;
        read(fd[i][0], &result, sizeof(result));
        total_result += result;
        close(fd[i][0]); // zamknięcie końca odczytującego
        waitpid(pid[i], NULL, 0); // czekanie na zakończenie procesu potomnego
    }

    clock_t end = clock();
    double time_spent = (double) (end - start) / CLOCKS_PER_SEC;

    printf("Wynik: %.10lf\n", total_result);
    printf("Czas obliczeń: %.6lf s\n", time_spent);

    // zapisz do pliku raportu
    FILE *report_file = fopen("raport.txt", "a");
    if (report_file != NULL) {
        fprintf(report_file, "%.10lf %d %.6lf\n", dx, num_procs, time_spent);
        fclose(report_file);
    }
    return 0;
}


