#pragma once
#include <QDialog>
//#include <QGroupBox>
#include <QPushButton>
#include <QLineEdit>
#include "PhysicalModel.h"

class Zview : public QDialog
{
    Q_OBJECT
public:
    Zview(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~Zview(void);

    void setModel(PhysicalModel*);

public slots:
    void updateModel();
    void modelChanged();
    void setCurrentZ(double d);

private:
    PhysicalModel *model;
    zParameters last;
//  QPushButton *bInit, *bFin;
    QLineEdit *leZ;
    QLineEdit *leZmin;
    QLineEdit *leZmax;
    QLineEdit *leHz;
};
