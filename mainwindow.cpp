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
#include <mntent.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <thread>

#include "mainwindow.h"

#define MILLION 1000000.0;
#define BILLION 1000000000.0
#define SIZE_LEN (15)

MainWindow::MainWindow() {
    widget = new QWidget;
    setCentralWidget(widget);

    QWidget *topFiller = new QWidget;
    topFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    infoLabel = new QLabel(tr("<i>Choose a menu option</i>"));
    infoLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    infoLabel->setAlignment(Qt::AlignCenter);

    textBrowser = new QTextBrowser();
/*
 * ---------------Resource page: CPU History---------------
 */
//    CPUHistory = new QLineSeries();
//    drawCPUHistoryGraph();

//    chart = new QChart();
//    chart->legend()->hide();
//    chart->addSeries(CPUHistory);
//    chart->createDefaultAxes();
//    chart->setTitle("CPU History");

//    chartView = new QChartView(chart);
//    chartView->setRenderHint(QPainter::Antialiasing);
/* --------------------------------------------------------*/

    QWidget *bottomFiller = new QWidget;
    bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    // vertical box layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);
    layout->addWidget(topFiller);
    layout->addWidget(infoLabel);
    layout->addWidget(textBrowser);
    textBrowser->hide();
    //layout->addWidget(chartView);
    //chartView->hide();
    layout->addWidget(bottomFiller);
    widget->setLayout(layout);


    createActions();
    createMenus();
// <<<<<<< file_system

//     QWidget *system = new QWidget();
//     basicInfo(system);

//     QWidget *fileSystems = new QWidget();
//     fileSystem(fileSystems);

//     QTabWidget *tabWidget = new QTabWidget(widget);
//     tabWidget->setFixedSize(720, 480);
//     tabWidget->addTab(system, tr("&System"));
//     tabWidget->addTab(new QWidget(), tr("&Processes"));
//     tabWidget->addTab(new QWidget(), tr("&Resources"));
//     tabWidget->addTab(fileSystems, tr("&File Systems"));


//     QString message = tr("Try right click");
//     statusBar()->showMessage(message);
// =======
    createTabs();

    setWindowTitle(tr("System Monitor"));
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

void MainWindow::basicInfo(QWidget *system) {
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);
    systemInfo = (char*) malloc(2048);
    bzero(systemInfo, 2048);
    showOSVersion();
    showKernelVersion();
    showMemoryStatus();
    showProcessorInfo();
    showDiskStorage();
    QLabel *title = new QLabel(systemInfo);

    layout->addWidget(title);
    system->setLayout(layout);
}

void MainWindow::fileSystem(QWidget *fileSystems) {
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);
    QVBoxLayout *innerLayout = new QVBoxLayout;
    innerLayout->setContentsMargins(4,4,4,4);
    FILE *fp = setmntent("/etc/mtab", "r");
    struct mntent *mounts;

    int numMounts = 0;
    while ((mounts = getmntent(fp)) != NULL) {
        numMounts++;
    }
    rewind(fp);

    int i = 0;
    QTableWidget *table = new QTableWidget;
    table->setRowCount(numMounts);
    table->setColumnCount(7);
    QStringList tableHeader;
    tableHeader<<"Device"<<"Directory"<<"Type"<<"Total"<<"Free"<<"Available"<<"Used";
    table->setHorizontalHeaderLabels(tableHeader);
    table->verticalHeader()->setVisible(false);
    while ((mounts = getmntent(fp)) != NULL) {
        struct statvfs stat;
        statvfs(mounts->mnt_dir, &stat);
        char *device = mounts->mnt_fsname;
        char *directory = mounts->mnt_dir;
        char *type = mounts->mnt_type;
        double total = stat.f_blocks * stat.f_frsize * 8.0 / 2097152;
        double free = stat.f_bfree * 8.0 / 2097152;
        double available = stat.f_bavail * 8.0 / 2097152;
        double used = (stat.f_bsize - stat.f_bfree) * 8.0 / 2097152;
        char *total_str = (char *) malloc(SIZE_LEN * sizeof(char));
        char *free_str = (char *) malloc(SIZE_LEN * sizeof(char));
        char *available_str = (char *) malloc(SIZE_LEN * sizeof(char));
        char *used_str = (char *) malloc(SIZE_LEN * sizeof(char));
        sprintf(total_str, "%10.1f GiB", total);
        sprintf(free_str, "%10.1f GiB", free);
        sprintf(available_str, "%10.1f GiB", available);
        sprintf(used_str, "%10.1f GiB", used);

        table->setItem(i, 0, new QTableWidgetItem(device));
        table->setItem(i, 1, new QTableWidgetItem(directory));
        table->setItem(i, 2, new QTableWidgetItem(type));
        table->setItem(i, 3, new QTableWidgetItem(total_str));
        table->setItem(i, 4, new QTableWidgetItem(free_str));
        table->setItem(i, 5, new QTableWidgetItem(available_str));
        table->setItem(i, 6, new QTableWidgetItem(used_str));
        i++;
    }
    fclose(fp);
    table->setVisible(true);
    innerLayout->addWidget(table);
    layout->addLayout(innerLayout);

    fileSystems->setLayout(layout);
}

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

    if (infoLabel->isHidden()) {
        infoLabel->show();
        textBrowser->hide();
    }
    strcat(systemInfo, "OS Version:\n");
    strcat(systemInfo, strdup(token));
}

void MainWindow::showKernelVersion() {
    FILE *f = fopen("/etc/os-release", "r");
    if (f == NULL) {
        cout << "Unable to open /etc/os-release" << endl;
        fclose(f);
    }

    char buffer[1024];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), f);
    fclose(f);

    if ((bytes_read == 0) || (bytes_read==sizeof(buffer))) {
        perror("Reading failed\n");
        exit(-1);
    }

    char version[50];
    bzero(version, 50);
    buffer[bytes_read] = '\0';

    // parse kernel version
    char *start = buffer;
    start += strlen("NAME==");
    char *end = start;
    end = strstr(end, "VERSION=");
    end -= 1;
    *end = '\0';
    strcat(version, start);
    start = end;
    start += strlen("VERSION===");
    end = start;
    end = strstr(end, "ID=");
    end -= 2;
    *end = '\0';
    strcat(version, start);

    if (infoLabel->isHidden()) {
        infoLabel->show();
        textBrowser->hide();
    }
    strcat(systemInfo, "\n\nKernel Version:\n");
    strcat(systemInfo, strdup(version));
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
    char *start = buffer;
    char *end = start;
    end = strstr(end, "Buffers:");
    *end = '\0';
    cout << start << endl;

    strcat(systemInfo, "\n\n");
    strcat(systemInfo, strdup(start));
}

void MainWindow::showProcessorInfo() {
    FILE *f = fopen("/proc/cpuinfo", "r");
    if (f == NULL) {
        cout << "Unable to open /proc/cpuinfo" << endl;
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
    char *end = strstr(buffer, "steppin");
    end -= 1;
    *end = '\0';

    if (infoLabel->isHidden()) {
        infoLabel->show();
        textBrowser->hide();
    }

    strcat(systemInfo, "\nProcessor Info:\n");
    strcat(systemInfo, strdup(start));
}

void MainWindow::showDiskStorage() {
    struct statvfs *stat = (struct statvfs *) malloc(sizeof(struct statvfs));
    statvfs("./main.cpp", stat);
    const unsigned int GB = 1024 * 1024 * 1024;
    double total = (double) (stat->f_blocks * stat->f_frsize) / GB;
    double avail = (double) (stat->f_bfree * stat->f_blocks) / GB;
    double used =  avail - total;
    double usedPercentage = (double) (used / avail) * (double) 100.0;
    free(stat);
    stat = NULL;

    char char_array[200];
    sprintf(char_array, "Total: %.2fGB\n"
    "Available: %.2fGB\n"
    "Used: %.2fGB\n"
    "Used Percentage: %.1f\%"
    "\0",
    avail, total, used, usedPercentage);

    if (infoLabel->isHidden()) {
        infoLabel->show();
        textBrowser->hide();
    }
    strcat(systemInfo, "\n\nDisk Storage:\n");
    strcat(systemInfo, strdup(char_array));

    QString message = tr("System Information");
    statusBar()->showMessage(message);
}

void MainWindow::showProcesses() {
//    system("ps -ef > temp.txt");
//    FILE *f = fopen("./temp.txt", "r");
//    fseek(f, 0, SEEK_END);
//    int size = ftell(f);
//    fseek(f, 0, SEEK_SET);
//    char char_array[size + 1];
//    fread(char_array, size, 1, f);
//    system("rm ./temp.txt");
//    char_array[size] = '\0';

//    //creating a list for the output
//    QList<QString> processList;

//    textBrowser->setText(char_array);
//    textBrowser->show();
//    infoLabel->hide();
//    QString message = tr("processes");
//    statusBar()->showMessage(message);
}

void MainWindow::drawCPUHistoryGraph() {
    FILE *cpuLog = fopen("../cpuLog.txt", "r");
    if (cpuLog == NULL) {
        perror("Unable to open cpuLog.txt");
    }
    char buffer[1024];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), cpuLog);
    fclose(cpuLog);

    if (bytes_read == 0) {
        perror("Reading failed\n");
        exit(-1);
    }

    char *token = strtok(buffer, "\n");
    for (int i = 0; i < 60; i++) {
        double pctg;
        sscanf(token, "%lf", &pctg);
        *CPUHistory << QPoint(i, pctg);
        cout << pctg << endl;
        token = strtok(NULL, "\n");
    }
}


void MainWindow::showCPUHistory() {
    chart->adjustSize();
    chartView->show();
    infoLabel->hide();
    textBrowser->hide();

    QString message = tr("CPU History");
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

    processAct = new QAction(tr("Processes"), this);
    connect(processAct, &QAction::triggered, this, &MainWindow::showProcesses);

    showCPUHistoryAct = new QAction(tr("CPU History"), this);
    connect(showCPUHistoryAct, &QAction::triggered, this, &MainWindow::showCPUHistory);
}

void MainWindow::createMenus() {
    infoMenu = menuBar()->addMenu(tr("&Monitor"));
    infoMenu = menuBar()->addMenu(tr("&Edit"));
    infoMenu = menuBar()->addMenu(tr("&View"));
    infoMenu = menuBar()->addMenu(tr("&Help"));
//    infoMenu = menuBar()->addMenu(tr("&Info"));
//    infoMenu->addAction(osVersionAct);
//    infoMenu->addAction(kernelVersionAct);
//    infoMenu->addAction(memoryStatusAct);
//    infoMenu->addAction(processorInfoAct);
//    infoMenu = menuBar()->addMenu(tr("&Info"));
//    infoMenu->addAction(osVersionAct);
//    infoMenu->addAction(kernelVersionAct);
//    infoMenu->addAction(memoryStatusAct);
//    infoMenu->addAction(processorInfoAct);
//    infoMenu->addAction(diskStorageAct);
  
//    infoMenu = menuBar()->addMenu(tr("&Process"));
//    infoMenu->addAction(processAct);

//    infoMenu = menuBar()->addMenu(tr("&Resources"));
//    infoMenu->addAction(showCPUHistoryAct);

//    infoMenu->addAction(); //USE FOR process
}

void MainWindow::createTabs() {
        system = new QWidget();
        basicInfo(system);
        processes = new QWidget();
        resources = new QWidget();
        fileSystem = new QWidget();

        tabWidget = new QTabWidget(widget);
        tabWidget->setFixedSize(720, 480);
        tabWidget->addTab(system, tr("&System"));
        tabWidget->addTab(processes, tr("&Processes"));
        tabWidget->addTab(resources, tr("&Resources"));
        tabWidget->addTab(fileSystem, tr("&File Systems"));
}
