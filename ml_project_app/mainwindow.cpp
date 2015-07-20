#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "zoommediator.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsPixmapItem>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow),
    _scene(new QGraphicsScene),
    _imagePixmapItem(_scene->addPixmap(QPixmap()))
{
    _ui->setupUi(this);
    _ui->graphicsView->setScene(_scene);

    // make mouse wheel perform zoom operations on the graphics view
    _zoomMediator = new ZoomMediator(_ui->graphicsView);

    // allow the user to draw scribbles
    _scribbleMediator = new ScribbleMediator(_ui->graphicsView);
    connect(_scribbleMediator, &ScribbleMediator::scribbleAdded, this, &MainWindow::on_scribbleAdded);

    // ensure that one of the image tools is active each time.
    QActionGroup* imageTools = new QActionGroup(this);
    imageTools->addAction(_ui->actionHand);
    imageTools->addAction(_ui->actionErase);
    imageTools->addAction(_ui->actionScribble);
    _ui->actionHand->setChecked(true);

    // select a label
    _ui->labelsTableWidget->selectRow(0);
}

MainWindow::~MainWindow()
{
    delete _ui;
}

void MainWindow::on_actionOpenImage_triggered()
{
    auto fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open image"),
        QString(),
        tr("Image files (*.jpg *.jpeg *.png);;All files (*.*)")
    );
    if (fileName.size() > 0)
    {
        QPixmap pixmap;
        if (pixmap.load(fileName))
        {
            clearAllScribbles();
            showPixmapFitInView(pixmap);
            enableDisableScribble();

            setImageForSegmentation(pixmap);
            displaySegmentation();
        }
        else
        {
            QMessageBox::warning(
                this,
                tr("Error"),
                tr("Unable to open image %1").arg(fileName)
            );
        }
    }
}


void MainWindow::on_actionScribble_toggled(bool checked)
{
    if (checked)
    {
        _ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
        _zoomMediator->SetEnabled(false);
        _scribbleMediator->setEnabled(true);
    }
}

void MainWindow::on_actionErase_toggled(bool checked)
{
    if (checked)
    {
        _ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
        _zoomMediator->SetEnabled(false);
        _scribbleMediator->setEnabled(false);
    }
}

void MainWindow::on_actionHand_toggled(bool checked)
{
    if (checked)
    {
        _ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
        _zoomMediator->SetEnabled(true);
        _scribbleMediator->setEnabled(false);
    }
}

void MainWindow::on_labelsTableWidget_itemSelectionChanged()
{
    enableDisableScribble();

    for(auto item : _ui->labelsTableWidget->selectedItems())
    {
        auto row = item->row();
        auto colorItem = _ui->labelsTableWidget->item(row, 0);
        _scribbleMediator->setColor(colorItem->backgroundColor());
    }
}

void MainWindow::on_scribbleAdded(QGraphicsPathItem *pi)
{
    addScribbleToSegmentationEngine(pi);
    displaySegmentation();
}

void MainWindow::clearAllScribbles()
{
    for(auto item : _ui->graphicsView->scene()->items())
        if (item != _imagePixmapItem)
            _ui->graphicsView->scene()->removeItem(item);
}

void MainWindow::showPixmapFitInView(QPixmap pixmap)
{
    _imagePixmapItem->setPixmap(pixmap);
    _ui->graphicsView->scene()->setSceneRect(_imagePixmapItem->boundingRect());
    _ui->graphicsView->fitInView(_imagePixmapItem);
}

void MainWindow::enableDisableScribble()
{
    if (_ui->labelsTableWidget->selectedItems().empty() || _imagePixmapItem->pixmap().isNull())
    {
        _ui->actionScribble->setEnabled(false);
        _ui->actionHand->setChecked(true);
    }
    else
        _ui->actionScribble->setEnabled(true);
}

void MainWindow::addScribbleToSegmentationEngine(QGraphicsPathItem *pi)
{
    for(auto item : _ui->labelsTableWidget->selectedItems())
    {
        auto labelId = item->row();
        _segmentationEngine.addScribble(pi->path(), labelId);
    }
}

void MainWindow::setImageForSegmentation(QPixmap pixmap)
{
    _segmentationEngine.reset(pixmap.toImage());
}

void MainWindow::displaySegmentation()
{
    QPixmap pixmap = _imagePixmapItem->pixmap();
    QPainter painter(&pixmap);
    for(auto label : _segmentationEngine.getLabels())
    {
        auto labelColor = _ui->labelsTableWidget->item(label, 0)->backgroundColor();

        auto mask = _segmentationEngine.getMaskOf(label).toImage();
        mask.setColor(0, qRgba(0, 0, 0, 0));
        mask.setColor(1, qRgba(labelColor.red(), labelColor.green(), labelColor.blue(), 64));

        painter.drawImage(0, 0, mask);
    }

    _imagePixmapItem->setPixmap(pixmap);
}
