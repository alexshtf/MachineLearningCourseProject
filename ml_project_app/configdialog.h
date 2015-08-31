#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(class Config& config, QWidget *parent = 0);
    ~ConfigDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ConfigDialog *_ui;
    class Config& _config;
};

#endif // CONFIGDIALOG_H
