#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QTextBrowser>
#include <QtWidgets>

#include <QtCharts/QLineSeries>
#include <QChartView>
#include <QPointF>
#include <stack>

using namespace std;
using namespace QtCharts;

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QLabel;
class QMenu;
//class QList;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif // QT_NO_CONTEXTMENU

private slots:
    void showOSVersion();
    void showKernelVersion();
    void showMemoryStatus();
    void showProcessorInfo();
    void showDiskStorage();

    void showProcesses(QWidget*);

    void showCPUHistory();
    void drawCPUHistoryGraph();
    void drawMemoryGraph();
    void drawSwapGraph();
    void drawRecGraph();
    void drawSendGraph();
    void getTotalNetwork();

private:
    void createActions();
    void createMenus();
    void createTabs();
    void basicInfo(QWidget*);

    void resourcesPage(QWidget*);
    void showFileSystem(QWidget*);

    QWidget *widget;
    QMenu *infoMenu;

    QTabWidget *tabWidget;
    QWidget *system;
    QWidget *processes;
    QWidget *resources;
    QWidget *fileSystem;

    char *systemInfo;

    QAction *osVersionAct;
    QAction *kernelVersionAct;
    QAction *memoryStatusAct;
    QAction *processorInfoAct;
    QAction *diskStorageAct;
    QAction *processAct;
    QAction *showCPUHistoryAct;

    QLabel *infoLabel;
    QTextBrowser *textBrowser;
    QPushButton *refresh;


/* ---- Resource page ---------*/
    QChart *chart;
    QChartView *chartView;
    QLineSeries *CPUHistory;

    QChart *memChart;
    QChartView *memChartView;
    QLineSeries *memHistory;

    QChart *swapChart;
    QChartView *swapChartView;
    QLineSeries *swapHistory;

    QChart *recChart;
    QChartView *recChartView;
    QLineSeries *recHistory;

    QChart *sendChart;
    QChartView *sendChartView;
    QLineSeries *sendHistory;
    char *totalNet;
};
#endif
