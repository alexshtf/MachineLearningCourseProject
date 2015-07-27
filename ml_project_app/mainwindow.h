#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "zoommediator.h"
#include "scribblemediator.h"
#include "segmentationengine.h"
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
    void on_actionSaveSimilarityMap_triggered();
    void on_labelsTableWidget_itemSelectionChanged();

    void on_scribbleAdded(QGraphicsPathItem* pi);

private:
    void enableDisableScribble();
    void addScribbleToSegmentationEngine(QGraphicsPathItem *pi);
    void setImageForSegmentation(QPixmap pixmap);
    void displaySegmentation();

    Ui::MainWindow* _ui;
    QPixmap _segmentedImage;
    QGraphicsScene* _scene;
    QGraphicsPixmapItem* _imagePixmapItem;

    ZoomMediator* _zoomMediator;
    ScribbleMediator* _scribbleMediator;
    SegmentationEngine _segmentationEngine;
};

#endif // MAINWINDOW_H
