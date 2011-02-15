#pragma once
#include <QDialog>
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
//    void invalidateModel();

private:
    PhysicalModel *model;
    zParameters last;

    QLineEdit *leZ;
    QLineEdit *leZmin;
    QLineEdit *leZmax;
    QLineEdit *leHz;
};
