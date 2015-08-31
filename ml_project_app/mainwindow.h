#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "zoommediator.h"
#include "scribblemediator.h"
#include "segmentationengine.h"
#include <QMainWindow>
#include <memory>
#include <chrono>

namespace Ui {
class MainWindow;
}

class QGraphicsScene;
class QGraphicsPixmapItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(class Config& config, SegmentationEngine* segmentationEngine,  QWidget *parent = 0);
    ~MainWindow();

private slots:
    // events from the UI
    void on_actionOpenImage_triggered();

    void on_actionScribble_toggled(bool checked);
    void on_actionErase_toggled(bool arg1);
    void on_actionHand_toggled(bool arg1);
    void on_actionSaveSimilarityMap_triggered();
    void on_labelsTableWidget_itemSelectionChanged();

    void on_scribbleAdded(QGraphicsPathItem* pi);
    void on_addLabelButton_clicked();
    void on_actionRecompute_triggered();
    void on_actionConfigure_triggered();

    // events from segmentation engine
    void onRecomputeDone();

private:
    void clearAllScribbles();
    void showPixmapFitInView(QPixmap pixmap);
    void enableDisableScribble();
    void addScribbleToSegmentationEngine(QGraphicsPathItem *pi);
    void setImageForSegmentation(QPixmap pixmap);
    void displaySegmentation();

    void log(const QString& message);

    Ui::MainWindow* _ui;

    Config& _config;

    QPixmap _segmentedImage;
    QGraphicsScene* _scene;
    QGraphicsPixmapItem* _imagePixmapItem;

    ZoomMediator* _zoomMediator;
    ScribbleMediator* _scribbleMediator;
    SegmentationEngine* _segmentationEngine;

    std::chrono::high_resolution_clock::time_point _epoch;
};

#endif // MAINWINDOW_H
