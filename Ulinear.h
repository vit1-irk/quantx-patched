#pragma once
#include <QDialog>
#include <QLineEdit>
#include "PhysicalModel.h"

class Ulinear : public QDialog
{
    Q_OBJECT
public:
    Ulinear(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~Ulinear(void);

    void setModel(PhysicalModel*);

public slots:
    void updateModel();
    void modelChanged();

private:
    PhysicalModel *model;
//    UAsMW last;
    double Ubiaslast;

    QLineEdit *leUBias;
};
