#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QLabel;
class QMenu;
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

private:
    void createActions();
    void createMenus();

    QMenu *infoMenu;

    QAction *osVersionAct;
    QAction *kernelVersionAct;
    QAction *memoryStatusAct;
    QAction *processorInfoAct;
    QAction *diskStorageAct;

    QLabel *infoLabel;

};
#endif
