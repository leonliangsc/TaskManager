#include <QtWidgets>
#include <iostream>
#include <string>
#include <sstream>
#include <streambuf>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <fstream>
#include <proc_service.h>
#include <sys/statvfs.h>
#include <sys/types.h>

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

    QWidget *bottomFiller = new QWidget;
    bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // vertical box layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);
    layout->addWidget(topFiller);
    layout->addWidget(infoLabel);
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
    FILE *f = fopen("/proc/version", "r");
    if (f == NULL) {
        cout << "Unable to open /proc/version" << endl;
        fclose(f);
    }

    char buffer[2048];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), f);
    fclose(f);

    if ((bytes_read == 0) || (bytes_read==sizeof(buffer))) {
        perror("Reading failed\n");
        exit(-1);
    }

    buffer[bytes_read] = '\0';
    char *token = strtok(buffer, "(");

    infoLabel->setText(token);
    QString message = tr("OS version");
    statusBar()->showMessage(message);
}

void MainWindow::showKernelVersion() {
    FILE *f = fopen("/proc/sys/kernel/version", "r");
    if (f == NULL) {
        cout << "Unable to open /proc/sys/kernel/version" << endl;
        fclose(f);
    }

    char buffer[2048];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), f);
    fclose(f);

    if ((bytes_read == 0) || (bytes_read==sizeof(buffer))) {
        perror("Reading failed\n");
        exit(-1);
    }

    buffer[bytes_read] = '\0';
    char *start = buffer;
    char *end = buffer;
    end = strchr(buffer, '\n');
    buffer[*end] = '\0';

    infoLabel->setText(start);
    QString message = tr("kernel version");
    statusBar()->showMessage(message);
}

void MainWindow::showMemoryStatus() {
    FILE *f = fopen("/proc/meminfo", "r");
    if (f == NULL) {
        cout << "Unable to open /proc/meminfo" << endl;
        fclose(f);
    }

    char buffer[2048];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), f);
    fclose(f);

    if ((bytes_read == 0) || (bytes_read==sizeof(buffer))) {
        perror("Reading failed\n");
        exit(-1);
    }

    buffer[bytes_read] = '\0';
    char *token = buffer;

    infoLabel->setText(token);
    QString message = tr("memory status");
    statusBar()->showMessage(message);
}

void MainWindow::showProcessorInfo() {
    FILE *f = fopen("/proc/cpuinfo", "r");
    if (f == NULL) {
        cout << "Unable to open /proc/cpuinfo" << endl;
        fclose(f);
    }

    char buffer[2048];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), f);
    fclose(f);

    if (bytes_read == 0) {
        perror("Reading failed\n");
        exit(-1);
    }

    buffer[bytes_read] = '\0';
    char *start = strstr(buffer, "model name");
    start += strlen("model name : ");
    char *end = strstr(buffer, "stepping");
    *end = '\0';

    infoLabel->setText(start);
    QString message = tr("processor information");
    statusBar()->showMessage(message);
}

void MainWindow::showDiskStorage() {
    struct statvfs *stat = (struct statvfs *) malloc(sizeof(struct statvfs));
    statvfs("./main.cpp", stat);
    const unsigned int GB = 1024 * 1024 * 1024;
    double total = (double) (stat->f_blocks * stat->f_frsize) / GB;
    double avail = (double) (stat->f_bfree * stat->f_blocks) / GB;
    double used = total - avail;
    double usedPercentage = (double) (used / total) * (double) 100.0;

    char char_array[200];
    sprintf(char_array, "Total: %.2fGB\n"
    "Available: %.2fGB\n"
    "Used: %.2fGB\n"
    "Used Percentage: %.1f\%"
    "\0",
    total, avail, used, usedPercentage);

    infoLabel->setText(char_array);
    QString message = tr("available disk storage");
    statusBar()->showMessage(message);
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

    diskStorageAct = new QAction(tr("Disk Storage"), this);
    connect(diskStorageAct, &QAction::triggered, this, &MainWindow::showDiskStorage);
}

void MainWindow::createMenus() {
    infoMenu = menuBar()->addMenu(tr("&Info"));
    infoMenu->addAction(osVersionAct);
    infoMenu->addAction(kernelVersionAct);
    infoMenu->addAction(memoryStatusAct);
    infoMenu->addAction(processorInfoAct);
    infoMenu->addAction(diskStorageAct);
}
