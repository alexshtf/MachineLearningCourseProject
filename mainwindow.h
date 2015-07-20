#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "zoommediator.h"
#include "scribblemediator.h"
#include "scribblemaskgenerator.h"
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
    void clearAllScribbles();
    void showPixmapFitInView(QPixmap pixmap);

    void on_actionScribble_toggled(bool checked);
    void on_actionErase_toggled(bool arg1);
    void on_actionHand_toggled(bool arg1);
    void on_labelsTableWidget_itemSelectionChanged();

    void enableDisableScribble();

    void scribbleAdded(QGraphicsPathItem* pi);

private:
    Ui::MainWindow* _ui;
    QGraphicsScene* _scene;
    QGraphicsPixmapItem* _imagePixmapItem;
    ZoomMediator* _zoomMediator;
    ScribbleMediator* _scribbleMediator;
    ScribbleMaskGenerator _scribbleMaskGenerator;
};

#endif // MAINWINDOW_H
