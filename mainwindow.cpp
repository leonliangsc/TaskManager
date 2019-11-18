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

    QString message = tr("Try right click");
    statusBar()->showMessage(message);

    setWindowTitle(tr("Task Manager"));
    setMinimumSize(160, 160);
    resize(480, 320);
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
    system("system_profiler SPSoftwareDataType | grep \"System Version\" > temp.txt");
    FILE *f = fopen("./temp.txt", "r");
    char char_array[30];
    fread(char_array, 30, 1, f);
    system("rm ./temp.txt");

    infoLabel->setText(char_array);
    QString message = tr("OS version");
    statusBar()->showMessage(message);
}

void MainWindow::showKernelVersion() {
    system("system_profiler SPSoftwareDataType | grep \"Kernel Version\" > temp.txt");
    FILE *f = fopen("./temp.txt", "r");
    char char_array[35];
    fread(char_array, 35, 1, f);
    system("rm ./temp.txt");

    infoLabel->setText(char_array);
    QString message = tr("kernel version");
    statusBar()->showMessage(message);
}

void MainWindow::showMemoryStatus() {
    infoLabel->setText(tr("Memory left: infinity")); //TODO: remove hardcode
    QString message = tr("memory status");
    statusBar()->showMessage(message);
}

void MainWindow::showProcessorInfo() {
    infoLabel->setText(tr("3.4 GHz Quad-Core Intel Core i5")); //TODO: remove hardcode
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
    infoMenu = menuBar()->addMenu(tr("&Info"));
    infoMenu->addAction(osVersionAct);
    infoMenu->addAction(kernelVersionAct);
    infoMenu->addAction(memoryStatusAct);
    infoMenu->addAction(processorInfoAct);
}
