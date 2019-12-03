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
#include <iostream>

#include "mainwindow.h"

using namespace std;

unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;
int receive;
int transmit;


void recordCPUUsage() {
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

    fprintf(cpuLog, "%.2f\n", percent);
    fclose(cpuLog);
}

void recordMemSwapUsage() {
    FILE *memInfo = fopen("/proc/meminfo", "r");
    if (memInfo == NULL) {
        cerr << "Error opening /proc/meminfo" << endl;
        exit(-1);
    }

    int memTotal = 0;
    int memAvail = 0;
    int swapTotal = 0;
    int swapFree = 0;

    // get stats
    char temp[50];
    fscanf(memInfo, "MemTotal: %d kB\n", &memTotal);
    fgets(temp, 50, memInfo);
    fscanf(memInfo, "MemAvailable: %d kB\n", &memAvail);
    for (int i = 0; i < 11; i++) {
        fgets(temp, 50, memInfo);
    }
    fscanf(memInfo, "SwapTotal: %d kB\n", &swapTotal);
    fscanf(memInfo, "SwapFree: %d kB\n", &swapFree);
    fclose(memInfo);

    double memPctg = (double) (memTotal - memAvail) / memTotal * 100;
    double swapPctg = (double) (swapTotal - swapFree) / swapTotal * 100;

    // open files and write stats
    FILE *memLog = fopen("../memLog.txt", "a");
    FILE *swapLog = fopen("../swapLog.txt", "a");
    if ((memLog == NULL) || (swapLog == NULL)) {
        exit(-1);
    }

    fprintf(memLog, "%.2f\n", memPctg);
    fclose(memLog);
    fprintf(swapLog, "%.2f\n", swapPctg);
    fclose(swapLog);
}

void recordNetUsage() {
    int newReceive = 0;
    int newTransmit = 0;
    int holder = 0;

    // read stats
    FILE *net = fopen("/proc/net/dev", "r");
    char temp[100];
    fgets(temp, 100, net);
    fgets(temp, 100, net);
    fgets(temp, 100, net);
    fscanf(net, " eth0: %d %d %d %d %d %d %d %d %d",
           &newReceive, &holder, &holder, &holder, &holder, &holder, &holder, &holder,
           &newTransmit);
    fclose(net);

    // parse
    double receiveSpeed = (double) (newReceive - receive) / 3 / 1024;
    double transmitSpeed = (double) (newTransmit - transmit) / 3 / 1024;
    receive = newReceive;
    transmit = newTransmit;
    double totalReceived = (double) receive / 1024 / 1024 / 1024; // gB
    double totalTransmitted = (double) transmit / 1024 / 1024 / 1024; // gB


    // write
//    cout << "Receive speed: " << receiveSpeed << endl;
//    cout << "Transimit speed: " << transmitSpeed << endl;
//    cout << "Total received: " << totalReceived << endl;
//    cout << "Total transimited: " << totalTransmitted << endl;

    FILE *total = fopen("../netTotal.txt", "w");
    fprintf(total, "%.2f %.2f", totalReceived, totalTransmitted);
    fclose(total);

    FILE *perSec = fopen("../netSpeed.txt", "a");
    fprintf(perSec, "%.2f %.2f\n", receiveSpeed, transmitSpeed);
    fclose(perSec);
}

void init(){
    // init CPU usage
    FILE* file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &lastTotalUser, &lastTotalUserLow,
        &lastTotalSys, &lastTotalIdle);
    fclose(file);

    // init network usage
    FILE *net = fopen("/proc/net/dev", "r");
    char temp[100];
    fgets(temp, 100, net);
    fgets(temp, 100, net);
    fgets(temp, 100, net);

    receive = 0;
    transmit = 0;
    int holder = 0;
    fscanf(net, " eth0: %d %d %d %d %d %d %d %d %d",
           &receive, &holder, &holder, &holder, &holder, &holder, &holder, &holder,
           &transmit);

    // overwrite previous log
//    FILE *log = fopen("./cpuLog.txt", "w");
//    FILE *memLog = fopen("../memLog.txt", "w");
//    FILE *swapLog = fopen("../swapLog.txt", "w");
//    if ((log == NULL) || (memLog == NULL) || (swapLog == NULL)) {
//        exit(-1);
//    }
//    fclose(log);
//    fclose(memLog);
//    fclose(swapLog);

    // record
    while(true) {
        sleep(3);
        recordCPUUsage();
        recordMemSwapUsage();
        recordNetUsage();
    }

}


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    int pid = fork();
    if (pid < 0) {
        perror("fork error");
        exit(-1);
    }
    if (pid == 0) {
        init(); // start recording stats
    } else {
        window.show();
        kill(pid, SIGKILL);
    }
    return app.exec();
//    init();
//    return 1;
}
