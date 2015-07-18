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

    // ensure that one of the image tools is active each time.
    QActionGroup* imageTools = new QActionGroup(this);
    imageTools->addAction(_ui->actionHand);
    imageTools->addAction(_ui->actionErase);
    imageTools->addAction(_ui->actionScribble);
    _ui->actionHand->setChecked(true);
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
            _imagePixmapItem->setPixmap(pixmap);
            _ui->graphicsView->scene()->setSceneRect(_imagePixmapItem->boundingRect());
            _ui->graphicsView->fitInView(_imagePixmapItem);
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
    auto selectedItems = _ui->labelsTableWidget->selectedItems();
    for(auto item : selectedItems)
    {
        auto row = item->row();
        auto colorItem = _ui->labelsTableWidget->item(row, 0);
        _scribbleMediator->setColor(colorItem->backgroundColor());
    }
}
