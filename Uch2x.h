#pragma once
#include <QDialog>
#include <QLineEdit>
#include "PhysicalModel.h"

class Uch2x : public QDialog
{
    Q_OBJECT
public:
    Uch2x(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~Uch2x(void);

    void setModel(PhysicalModel*);

public slots:
    void updateModel();
    void modelChanged();

private:
    PhysicalModel *model;
    U_ch2x last;

    QLineEdit *leNumberOfIntervals;
    QLineEdit *leU0;
    QLineEdit *leWidth;
};
