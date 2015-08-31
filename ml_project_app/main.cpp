#include "mainwindow.h"
#include "config.h"
#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QSettings settings(QSettings::UserScope, "Alex Shtof", "Machine Learning Project");
    Config config(settings);

    QApplication a(argc, argv);
    MainWindow w(config);
    w.show();

    return a.exec();
}
