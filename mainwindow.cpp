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

    QWidget *bottomFiller = new QWidget;
    bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    // vertical box layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);
    layout->addWidget(topFiller);
    layout->addWidget(infoLabel);
    layout->addWidget(textBrowser);
    textBrowser->hide();
    layout->addWidget(bottomFiller);
    widget->setLayout(layout);


    createActions();
    createMenus();
    createTabs();

    setWindowTitle(tr("Task Manager"));
    setMinimumSize(980, 980);
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

void MainWindow::resourcesPage(QWidget *resources) {
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);

    QLabel *title = new QLabel(tr("Resources page"));
    /*
     * ---------------Resource page: CPU History---------------
     */
    CPUHistory = new QLineSeries();
    drawCPUHistoryGraph();

    chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(CPUHistory);
    chart->createDefaultAxes();
    chart->setTitle("CPU History");

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    showCPUHistory();
    /* --------------------------------------------------------*/
    memHistory = new QLineSeries();
    drawMemoryGraph();

    memChart = new QChart();
    memChart->legend()->hide();
    memChart->addSeries(memHistory);
    memChart->createDefaultAxes();
    memChart->setTitle("Memory History");

    memChartView = new QChartView(memChart);
    memChartView->setRenderHint(QPainter::Antialiasing);

    /*----------------------------------------------------------*/
    swapHistory = new QLineSeries();
    drawSwapGraph();

    swapChart = new QChart();
    swapChart->legend()->hide();
    swapChart->addSeries(swapHistory);
    swapChart->createDefaultAxes();
    swapChart->setTitle("Swap History");

    swapChartView = new QChartView(swapChart);
    swapChartView->setRenderHint(QPainter::Antialiasing);

    /*----------------------------------------------------------*/
    recHistory = new QLineSeries();
    drawRecGraph();
    recChart = new QChart();
    recChart->legend()->hide();
    recChart->addSeries(recHistory);
    recChart->createDefaultAxes();
    recChart->setTitle("Receiving History");

    recChartView = new QChartView(recChart);
    recChartView->setRenderHint(QPainter::Antialiasing);


    /*----------------------------------------------------------*/
    sendHistory = new QLineSeries();
    drawSendGraph();

    sendChart = new QChart();
    sendChart->legend()->hide();
    sendChart->addSeries(sendHistory);
    sendChart->createDefaultAxes();
    sendChart->setTitle("Sending History");

    sendChartView = new QChartView(sendChart);
    sendChartView->setRenderHint(QPainter::Antialiasing);

    getTotalNetwork();
    QLabel *total = new QLabel(totalNet);

    layout->addWidget(title);
    layout->addWidget(chartView);
    layout->addWidget(memChartView);
    layout->addWidget(swapChartView);
    layout->addWidget(recChartView);
    layout->addWidget(sendChartView);
    layout->addWidget(total);
    resources->setLayout(layout);
}

void MainWindow::showFileSystem(QWidget *fileSystems) {
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
        perror("Reading /etc/os-release failed\n");
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
        perror("Reading /proc/meminfo failed\n");
        exit(-1);
    }

    buffer[bytes_read] = '\0';
    char *start = buffer;
    char *end = start;
    end = strstr(end, "Buffers:");
    *end = '\0';
//    cout << start << endl;

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
        perror("Reading /proc/cpuinfo failed\n");
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
        perror("Reading ./cpuLog.txt failed\n");
        exit(-1);
    }

    char *token = strtok(buffer, "\n");
    if (token == NULL) {
        for (int i = 0; i < 60; i++) {
            *CPUHistory << QPoint(i, 0);
        }
    } else {
        for (int i = 0; i < 60; i++) {
            double pctg;
            if (token == NULL) {
                *CPUHistory << QPoint(i, 0);
            } else {
                sscanf(token, "%lf", &pctg);
                *CPUHistory << QPoint(i, pctg);
                token = strtok(NULL, "\n");
            }
        }
    }
}

void MainWindow::drawMemoryGraph() {
    FILE *cpuLog = fopen("../memLog.txt", "r");
    if (cpuLog == NULL) {
        perror("Unable to open memLog.txt");
    }
    char buffer[1024];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), cpuLog);
    fclose(cpuLog);

//    if (bytes_read == 0) {
//        perror("Reading ../memLog.txt failed\n");
//        exit(-1);
//    }

    char *token = strtok(buffer, "\n");
    if (token == NULL) {
        for (int i = 0; i < 60; i++) {
            *memHistory << QPoint(i, 0);
        }
    } else {
        for (int i = 0; i < 60; i++) {
            double pctg;
            if (token == NULL) {
                *memHistory << QPoint(i, 0);
            } else {
                sscanf(token, "%lf", &pctg);
                *memHistory << QPoint(i, pctg);
                token = strtok(NULL, "\n");
            }
        }
    }
}

void MainWindow::drawSwapGraph() {
    FILE *cpuLog = fopen("../swapLog.txt", "r");
    if (cpuLog == NULL) {
        perror("Unable to open swapLog.txt");
    }
    char buffer[1024];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), cpuLog);
    fclose(cpuLog);

//    if (bytes_read == 0) {
//        perror("Reading swapLog.txt failed\n");
//        exit(-1);
//    }

    char *token = strtok(buffer, "\n");
    if (token == NULL) {
        for (int i = 0; i < 60; i++) {
            *swapHistory << QPoint(i, 0);
        }
    } else {
        for (int i = 0; i < 60; i++) {
            double pctg;
            if (token == NULL) {
                *swapHistory << QPoint(i, 0);
            } else {
                sscanf(token, "%lf", &pctg);
                *swapHistory << QPoint(i, pctg);
                token = strtok(NULL, "\n");
            }
        }
    }
}


void MainWindow::drawRecGraph() {
    FILE *cpuLog = fopen("../netSpeed.txt", "r");
    if (cpuLog == NULL) {
        perror("Unable to open netSpeed.txt");
    }
    char buffer[1024];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), cpuLog);
    fclose(cpuLog);

    if (bytes_read == 0) {
        perror("Reading failed\n");
        exit(-1);
    }

    char *token = strtok(buffer, "\n");
    if (token == NULL) {
        for (int i = 0; i < 60; i++) {
            *recHistory << QPoint(i, 0);
        }
    } else {
        for (int i = 0; i < 60; i++) {
            double pctg;
            if (token == NULL) {
                *recHistory << QPoint(i, 0);
            } else {
                double rec, send;
                sscanf(token, "%lf %lf", &rec, &send);
                *recHistory << QPoint(i, rec);
                token = strtok(NULL, "\n");
            }
        }
    }
}

void MainWindow::drawSendGraph() {
    FILE *cpuLog = fopen("../netSpeed.txt", "r");
    if (cpuLog == NULL) {
        perror("Unable to open netSpeed.txt");
    }
    char buffer[1024];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), cpuLog);
    fclose(cpuLog);

    if (bytes_read == 0) {
        perror("Reading netSpeed.txt failed\n");
        exit(-1);
    }

    char *token = strtok(buffer, "\n");
    if (token == NULL) {
        for (int i = 0; i < 60; i++) {
            *sendHistory << QPoint(i, 0);
        }
    } else {
        for (int i = 0; i < 60; i++) {
            double pctg;
            if (token == NULL) {
                *sendHistory << QPoint(i, 0);
            } else {
                double rec, send;
                sscanf(token, "%lf %lf", &rec, &send);
                *sendHistory << QPoint(i, send);
                token = strtok(NULL, "\n");
            }
        }
    }
}

void MainWindow::getTotalNetwork() {
    FILE *total = fopen("../netTotal.txt", "r");
    if (total == NULL) {
        perror("Unable to open netTotal.txt");
    }
    totalNet = (char *)malloc(48);
    char buffer[1024];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), total);
    fclose(total);

    if (bytes_read == 0) {
        perror("Reading netTotal.txt failed\n");
        exit(-1);
    }
    double totalRec, totalSend;
    sscanf(buffer, "%lf %lf", &totalRec, &totalSend);
    sprintf(totalNet, "Total received: %.2fGB\tTotal sent: %.2fGB\n", totalRec, totalSend);
//    cout << totalNet << endl;
}

void MainWindow::showCPUHistory() {
    chart->adjustSize();
    chartView->show();
    infoLabel->hide();
    textBrowser->hide();

    QString message = tr("Resources");
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
}

void MainWindow::createTabs() {
        system = new QWidget();
        basicInfo(system);

        processes = new QWidget();

        resources = new QWidget();
        resourcesPage(resources);

        fileSystem = new QWidget();
        showFileSystem(fileSystem);

        tabWidget = new QTabWidget(widget);
        tabWidget->setFixedSize(980, 980);
        tabWidget->addTab(system, tr("&System"));
        tabWidget->addTab(processes, tr("&Processes"));
        tabWidget->addTab(resources, tr("&Resources"));
        tabWidget->addTab(fileSystem, tr("&File Systems"));
}
