#include "mainwindow.h"
#include "config.h"
#include "metatypes.h"
#include "interactivesegmentationcontroller.h"
#include "segmentationengine.h"
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
    SegmentationEngine engine(config);
    engine.moveToThread(&workerThread);
    workerThread.start();

    InteractiveSegmentationController controller(config, &engine);
    MainWindow w(config, &controller);
    w.show();

    auto returnCode = a.exec();

    workerThread.quit();
    workerThread.wait();

    return returnCode;
}
