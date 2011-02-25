#pragma once
#include <QDialog>
#include <QLineEdit>
#include "PhysicalModel.h"

class ScalePsin : public QDialog
{
    Q_OBJECT
public:
    ScalePsin(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~ScalePsin(void);

    void setModel(PhysicalModel*);

public slots:
    void updateModel();
    void modelChanged();

private:
    PhysicalModel *model;
    ScalePsinParameters last;

    QLineEdit *lePsimin;
    QLineEdit *lePsimax;
    QLineEdit *leXmin;
    QLineEdit *leXmax;
    QLineEdit *leHx;
};
