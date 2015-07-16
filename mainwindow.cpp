#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ensure that one of the image tools is active each time.
    QActionGroup* imageTools = new QActionGroup(this);
    imageTools->addAction(ui->actionHand);
    imageTools->addAction(ui->actionErase);
    imageTools->addAction(ui->actionScribble);
    ui->actionHand->setChecked(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpenImage_triggered()
{
    auto fileName = QFileDialog::getOpenFileName(
                this,
                tr("Open image"),
                QString(),
                tr("JPEG (*.jpg *.jpeg);;PNG (*.png)"));
    if (fileName.size() > 0)
    {

    }
}
