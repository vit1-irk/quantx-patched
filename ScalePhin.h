#pragma once
#include <QDialog>
#include <QLineEdit>
#include "PhysicalModel.h"

class ScalePhin : public QDialog
{
    Q_OBJECT
public:
    ScalePhin(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~ScalePhin(void);

    void setModel(PhysicalModel*);

public slots:
    void updateModel();
    void modelChanged();

private:
    PhysicalModel *model;
    ScalePhinParameters last;

    QLineEdit *lePhimin;
    QLineEdit *lePhimax;
    QLineEdit *leKmin;
    QLineEdit *leKmax;
    QLineEdit *leHk;
};
