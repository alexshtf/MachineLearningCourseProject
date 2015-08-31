#include "mainwindow.h"
#include "config.h"
#include "metatypes.h"
#include "segmentationengine.h"
#include "computesegmentationworker.h"
#include <QApplication>
#include <QThread>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    registerMetatypes();

    QSettings settings(QSettings::UserScope, "Alex Shtof", "Machine Learning Project");
    Config config(settings);

    QThread workerThread;
    ComputeSegmentationWorker worker(config);
    worker.moveToThread(&workerThread);
    workerThread.start();

    SegmentationEngine segmentationEngine(config, &worker);
    MainWindow w(config, &segmentationEngine);
    w.show();

    auto returnCode = a.exec();

    workerThread.quit();
    workerThread.wait();

    return returnCode;
}
