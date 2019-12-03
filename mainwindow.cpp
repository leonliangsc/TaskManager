#include <QtWidgets>
#include <iostream>
#include <string>
#include <sstream>
#include <streambuf>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <FL/Fl.H>

#include "mainwindow.h"

using namespace std;

MainWindow::MainWindow() {
    QWidget *widget = new QWidget;
    setCentralWidget(widget);

    QWidget *topFiller = new QWidget;
    topFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    infoLabel = new QLabel(tr("<i>Choose a menu option</i>"));
    infoLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    infoLabel->setAlignment(Qt::AlignCenter);
    textBrowser = new QTextBrowser();
    refresh = new QPushButton();
    refresh->setText(tr("refresh"));
    refresh->hide();


    QWidget *bottomFiller = new QWidget;
    bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // vertical box layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);
    layout->addWidget(topFiller);
    layout->addWidget(infoLabel);
    layout->addWidget(textBrowser);
    layout->addWidget(refresh);
    textBrowser->hide();
    layout->addWidget(bottomFiller);
    widget->setLayout(layout);

    createActions();
    createMenus();

    QString message = tr("Try right click");
    statusBar()->showMessage(message);

    setWindowTitle(tr("Task Manager"));
    setMinimumSize(480, 480);
    resize(720, 480);
}

#ifndef QT_NO_CONTEXTMENU
void MainWindow::contextMenuEvent(QContextMenuEvent *event) {
    // right click
    QMenu menu(this);
//    menu.addAction(cutAct);
    menu.exec(event->globalPos());
}
#endif // QT_NO_CONTEXTMENU

void MainWindow::showOSVersion() {
    system("cat /proc/version | head -c 101 > temp.txt");
    FILE *f = fopen("./temp.txt", "r");
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char char_array[size + 1];
    fread(char_array, size, 1, f);
    char_array[size] = '\0';
    system("rm ./temp.txt");

    if (infoLabel->isHidden()) {
        infoLabel->show();
        textBrowser->hide();
    }
    infoLabel->setText(char_array);
    QString message = tr("OS version");
    statusBar()->showMessage(message);
}

void MainWindow::showKernelVersion() {
    system("uname -r > temp.txt");
    FILE *f = fopen("./temp.txt", "r");
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char char_array[size + 1];
    fread(char_array, size, 1, f);
    system("rm ./temp.txt");
    char_array[size] = '\0';

    if (infoLabel->isHidden()) {
        infoLabel->show();
        textBrowser->hide();
    }
    infoLabel->setText(char_array);
    QString message = tr("kernel version");
    statusBar()->showMessage(message);
}

void MainWindow::showMemoryStatus() {
    system("cat /proc/meminfo | head -n 3 > temp.txt");
    FILE *f = fopen("./temp.txt", "r");
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char char_array[size + 1];
    fread(char_array, size, 1, f);
    system("rm ./temp.txt");
    char_array[size] = '\0';

    if (infoLabel->isHidden()) {
        infoLabel->show();
        textBrowser->hide();
    }
    infoLabel->setText(char_array);
    QString message = tr("memory status");
    statusBar()->showMessage(message);
}

void MainWindow::showProcessorInfo() {
    system("cat /proc/cpuinfo | grep \"model name\" | head -n 1 > temp.txt");
    FILE *f = fopen("./temp.txt", "r");
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char char_array[size + 1];
    fread(char_array, size, 1, f);
    system("rm ./temp.txt");
    char_array[size] = '\0';

    if (infoLabel->isHidden()) {
        infoLabel->show();
        textBrowser->hide();
    }
    infoLabel->setText(char_array);
    QString message = tr("processor information");
    statusBar()->showMessage(message);
}

void MainWindow::showProcesses() {


    //creating a list for the output
    QList<QString> processList;

    DIR* proc = opendir("/proc");
    struct dirent* ent;
    processList.append(" ");
    while((ent = readdir(proc)) != NULL){
        //sort out only the pid directories

        if(!isdigit(ent->d_name[0])) {
            continue;
        }
        fprintf(stderr, "%s\n", ent->d_name);
        char use[6];
        sprintf(use, "%s", (ent->d_name));


        char *stat = (char *) malloc((sizeof(char)) * (11 + strlen(use)));
        strcpy(stat, "/proc/");
        strcat(stat, use);
        strcat(stat, "/stat");
        fprintf(stderr, "%s\n", stat);
        fflush(stderr);

        FILE *pro = fopen(stat, "r");

        if (!pro) {
            //perror("unable to open file.\n");
            continue;
        }

        int pro_pid;
        char comm[100];
        char state;
        int ppid;
        int pgrp;
        int session;
        int tty_nr;
        int tpgid;
        unsigned int flags;
        unsigned long int minflt;
        unsigned long int cminflt;
        unsigned long int magflt;
        unsigned long int cmajflt;
        unsigned long int utime;
        unsigned long int stime;

        fscanf(pro, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu",
        &pro_pid, comm, &state, &ppid, &pgrp, &session, &tty_nr, &tpgid,
        &flags, &minflt, &cminflt, &magflt, &cmajflt, &utime, &stime);
        long int nouse;
        for (int i = 0; i < 6; i++) {
            fscanf(pro, " %ld", &nouse);
        }
        unsigned long long starttime;
        fscanf(pro, " %llu", &starttime);
        unsigned long vsize;
        fscanf(pro, " %lu", &vsize);


        fprintf(stderr, "%d %s %c %d %lu %lu", pro_pid, comm, state, ppid, utime, stime);

        /*
        fseek(pro, 0, SEEK_END);
        int size = ftell(pro);
        fseek(pro, 0, SEEK_SET);
        char char_array[size + 1];
        fread(char_array, size, 1, pro);

        fprintf(stderr, "%s\n", char_array);

        char **save = (char **) malloc(sizeof(char));
        strtok_r(char_array, "(", save);
        char* name = strtok_r(NULL, ")", save);
        */
        char check[6];
        sprintf(check, "%d", pro_pid);
        char mem[6];
        sprintf(mem, "%lu", vsize);
        QString list;
        list.append(comm);
        list.append("  ");
        list.append(check);
        if (state == 'S') {
            list.append("  sleeping  ");
        } else if (state == 'Z') {
            list.append("  zombie  ");
        } else if ((state == 'X') || (state == 'x')) {
            list.append("  dead  ");
        } else if (state == 'R') {
            list.append("  running  ");
        } else if (state == 'T') {
            list.append("  stopped  ");
        } else if (state == 't') {
            list.append("  tracing stop  ");
        }else if (state == 'I') {
            list.append("  Idle  ");
        }
        list.append(mem);

        list.append(QString("\n"));
        processList.append(list);

        //free(save);


        free(stat);
    }


    //redirect the qlist to string
    QString str;
    QDebug dStream(&str);
    dStream.noquote() << processList;
    str.remove(",");
    str.remove(0, 1);
    str.chop(2);


    textBrowser->setText(str);
    textBrowser->show();
    infoLabel->hide();
    refresh->show();
    QString message = tr("processes");
    statusBar()->showMessage(message);
    textBrowser->update();

}

void MainWindow::createActions() {
    osVersionAct = new QAction(tr("OS Version"), this);
    connect(osVersionAct, &QAction::triggered, this, &MainWindow::showOSVersion);

    kernelVersionAct = new QAction(tr("Kernel Version"), this);
    connect(kernelVersionAct, &QAction::triggered, this, &MainWindow::showKernelVersion);

    memoryStatusAct = new QAction(tr("Memory Status"), this);
    connect(memoryStatusAct, &QAction::triggered, this, &MainWindow::showMemoryStatus);

    processorInfoAct = new QAction(tr("Processor Info"), this);
    connect(processorInfoAct, &QAction::triggered, this, &MainWindow::showProcessorInfo);

    processAct = new QAction(tr("Processes"), this);
    connect(processAct, &QAction::triggered, this, &MainWindow::showProcesses);
    connect(refresh, SIGNAL(clicked()), this, SLOT(showProcesses()));
}

void MainWindow::createMenus() {
    infoMenu = menuBar()->addMenu(tr("&Info"));
    infoMenu->addAction(osVersionAct);
    infoMenu->addAction(kernelVersionAct);
    infoMenu->addAction(memoryStatusAct);
    infoMenu->addAction(processorInfoAct);
    infoMenu = menuBar()->addMenu(tr("&Process"));
    infoMenu->addAction(processAct);


    //infoMenu->addAction(); //USE FOR process
}
