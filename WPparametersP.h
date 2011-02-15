//#pragma once
#include <QDialog>
#include <QLineEdit>
#include "PhysicalModel.h"

class WPparametersP : public QDialog
{
    Q_OBJECT
public:
    WPparametersP(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~WPparametersP(void);

    void setModel(PhysicalModel*);

public slots:
    void updateModel();
    void modelChanged();

private:
    PhysicalModel *model;
    EpWP last;

    QLineEdit *leNumberOfModes;
    QLineEdit *leE_low;
    QLineEdit *leE_high;
};
