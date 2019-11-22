#include <QtWidgets>
#include <iostream>
#include <string>
#include <sstream>
#include <streambuf>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

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

    QWidget *system = new QWidget();
    basicInfo(system);

    QTabWidget *tabWidget = new QTabWidget(widget);
    tabWidget->setFixedSize(720, 480);
    tabWidget->addTab(system, tr("&System"));
    tabWidget->addTab(new QWidget(), tr("&Processes"));
    tabWidget->addTab(new QWidget(), tr("&Resources"));
    tabWidget->addTab(new QWidget(), tr("&File Systems"));


    QString message = tr("Try right click");
    statusBar()->showMessage(message);

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
    QLabel *title = new QLabel(tr("<i>Basic System Information</i>"));
    layout->addWidget(title);
    system->setLayout(layout);
}

void MainWindow::showOSVersion() {
    system("cat /proc/version | head -c 101 > temp.txt");
    FILE *f = fopen("./temp.txt", "r");
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char char_array[size];
    fread(char_array, size, 1, f);
    system("rm ./temp.txt");

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
    char char_array[size];
    fread(char_array, size, 1, f);
    system("rm ./temp.txt");

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
    char char_array[size];
    fread(char_array, size, 1, f);
    system("rm ./temp.txt");

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
    char char_array[size];
    fread(char_array, size, 1, f);
    system("rm ./temp.txt");

    infoLabel->setText(char_array);
    QString message = tr("processor information");
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
}
