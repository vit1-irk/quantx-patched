#pragma once
#include <QDialog>
#include <QLineEdit>
#include "PhysicalModel.h"

class ScalesUx : public QDialog
{
    Q_OBJECT
public:
    ScalesUx(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~ScalesUx(void);

    void setModel(PhysicalModel*);

public slots:
    void updateModel();
    void modelChanged();

private:
    PhysicalModel *model;
    ScalesUParameters last;
    QLineEdit *leUmin;
    QLineEdit *leUmax;
    QLineEdit *leXmin;
    QLineEdit *leXmax;
};
