#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "zoommediator.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsPixmapItem>

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
    connect(_scribbleMediator, &ScribbleMediator::scribbleAdded, this, &MainWindow::scribbleAdded);

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
            _scribbleMaskGenerator.setSize(pixmap.size());
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

void MainWindow::scribbleAdded(QGraphicsPathItem *pi)
{
    // add scribble
    _scribbleMaskGenerator.addScribble(pi->path());

    // draw the scribble over the image
    auto pixmap = _imagePixmapItem->pixmap();

    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::GlobalColor::blue, 4));

    auto pixels = _scribbleMaskGenerator.getPixels();
    painter.drawPoints(pixels.data(), pixels.size());

    _imagePixmapItem->setPixmap(pixmap);
}
