#ifndef COMPRESSDIALOG_H
#define COMPRESSDIALOG_H

#include <QtGui/QDialog>

#include "ui_compressdialog.h"


class CompressDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CompressDialog(const QString & fileName, QWidget *parent = 0);
    ~CompressDialog();

protected slots:

    void openClicked();


private:
    Ui::CompressDialog ui;
};

#endif // COMPRESSDIALOG_H
