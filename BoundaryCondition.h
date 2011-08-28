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
//    void readFromXml(QXmlStreamReader *r);
//    void writeToXml(QXmlStreamWriter *w);

public slots:
    void updateModel();
    void modelChanged();

private:
    PhysicalModel *model;
    QButtonGroup *bgR;
    PotentialType last;
};
