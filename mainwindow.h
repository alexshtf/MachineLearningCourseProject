#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

namespace Ui {
class MainWindow;
}

class QGraphicsScene;
class QGraphicsPixmapItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpenImage_triggered();

private:
    std::unique_ptr<Ui::MainWindow> _ui;
    QGraphicsScene* _scene;
    QGraphicsPixmapItem* _imagePixmapItem;
};

#endif // MAINWINDOW_H
