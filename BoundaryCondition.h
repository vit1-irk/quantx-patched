#pragma once
#include <QDialog>
#include <QWidget>
#include <QButtonGroup>
#include "PhysicalModel.h"

class BoundaryCondition : public QDialog
{
    Q_OBJECT
public:
    BoundaryCondition(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~BoundaryCondition(void);

    void setModel(PhysicalModel*);

public slots:
    void updateModel();
    void modelChanged();

private:
    PhysicalModel *model;
    QButtonGroup *bgR;
    PotentialType last;
};
