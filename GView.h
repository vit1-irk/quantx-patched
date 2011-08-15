#pragma once
#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include "PhysicalModel.h"

class GView : public QDialog
{
    Q_OBJECT
public:
    GView(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~GView(void);

    void setModel(PhysicalModel*);

public slots:
    void updateModel();
    void modelChanged();
    void setCurrentG(double d);

private:
    PhysicalModel *model;
    gParameters last;
    QLineEdit *leG;
    QLineEdit *leGmin;
    QLineEdit *leGmax;
    QLineEdit *leHG;
};
