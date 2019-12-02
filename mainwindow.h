#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QTextBrowser>
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

    void showProcesses();

    void showCPUHistory();
    void drawCPUHistoryGraph();

private:
    void createActions();
    void createMenus();
    void basicInfo(QWidget*);
    void fileSystem(QWidget*);

    QMenu *infoMenu;

    QAction *osVersionAct;
    QAction *kernelVersionAct;
    QAction *memoryStatusAct;
    QAction *processorInfoAct;
    QAction *diskStorageAct;
    QAction *processAct;
    QAction *showCPUHistoryAct;

    QLabel *infoLabel;
    QTextBrowser *textBrowser;

/* ---- Resource page ---------*/
    QChart *chart;
    QChartView *chartView;
    QLineSeries *CPUHistory;
};
#endif
