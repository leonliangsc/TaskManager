#include <QApplication>
#include <unistd.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <proc_service.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/signal.h>


#include "mainwindow.h"

unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;

void init(){
    FILE* file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &lastTotalUser, &lastTotalUserLow,
        &lastTotalSys, &lastTotalIdle);
//    fclose(file);
//    FILE *log = fopen("./cpuLog.txt", "w"); // overwrite previous log
//    if (log == NULL) {
//        exit(-1);
//    }
//    fprintf(log, "log\n");
//    fclose(log);
}

void recordCPUUsage() {
    init();

    while (true) {
        printf("Here");
        sleep(3);
        FILE *cpuLog = fopen("../cpuLog.txt", "a");
        if (cpuLog == NULL) {
            exit(-1);
        }
        double percent;
        FILE* file;
        unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

        file = fopen("/proc/stat", "r");
        fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
            &totalSys, &totalIdle);
        fclose(file);

        if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
            totalSys < lastTotalSys || totalIdle < lastTotalIdle) {
            //Overflow detection. Just skip this value.
            percent = -1.0;
        }
        else{
            total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
                (totalSys - lastTotalSys);
            percent = total;
            total += (totalIdle - lastTotalIdle);
            percent /= total;
            percent *= 100;
        }

        lastTotalUser = totalUser;
        lastTotalUserLow = totalUserLow;
        lastTotalSys = totalSys;
        lastTotalIdle = totalIdle;

        printf("%.2f\n", percent);
        fprintf(cpuLog, "%.2f\n", percent);
        fclose(cpuLog);
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
//    int pid = fork();
//    if (pid < 0) {
//        perror("fork error");
//        exit(-1);
//    }
//    if (pid == 0) {
//        recordCPUUsage();
//    } else {
        window.show();
//        kill(pid, SIGKILL);
//    }
//    recordCPUUsage();
    return app.exec();
}
