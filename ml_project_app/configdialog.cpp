#include "configdialog.h"
#include "ui_configdialog.h"
#include "config.h"

ConfigDialog::ConfigDialog(Config& config, QWidget *parent)
    : QDialog(parent)
    , _ui(new Ui::ConfigDialog)
    , _config(config)
{
    _ui->setupUi(this);

    _ui->neighborhoodSize->setValue(static_cast<int>(config.getNeighborhoodSize()));
    _ui->stoppingEpsilon->setText(QString::number(config.getStoppingEpsilon()));

    _ui->stoppingEpsilon->setValidator(new QDoubleValidator(0, 1, 17));

    resize(sizeHint());
}

ConfigDialog::~ConfigDialog()
{
    delete _ui;
}

void ConfigDialog::on_buttonBox_accepted()
{
    _config.setNeighborhoodSize(_ui->neighborhoodSize->value());
    _config.setStoppingEpsilon(_ui->stoppingEpsilon->text().toDouble());
    _config.save();
}
