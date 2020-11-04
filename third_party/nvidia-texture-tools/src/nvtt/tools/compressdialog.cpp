#include "compressdialog.h"
#include "ui_compressdialog.h"

#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    CompressDialog dialog("");

    return dialog.exec();
}



CompressDialog::CompressDialog(const QString & fileName, QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
}

CompressDialog::~CompressDialog()
{
}


void CompressDialog::openClicked()
{
    // @@ What is openButton?
}

