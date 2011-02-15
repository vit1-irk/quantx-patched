#pragma once
#include <QDialog>
#include <QLineEdit>
#include "PhysicalModel.h"

class Uparabolic : public QDialog
{
    Q_OBJECT
public:
    Uparabolic(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~Uparabolic(void);

    void setModel(PhysicalModel*);

public slots:
    void updateModel();
    void modelChanged();

private:
    PhysicalModel *model;
    Uparab last;

    QLineEdit *leNumberOfIntervals;
    QLineEdit *leU0;
    QLineEdit *leWidth;
};
