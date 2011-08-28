#pragma once
#include <QDialog>
#include <QLineEdit>
#include "PhysicalModel.h"

class TimeView : public QDialog
{
    Q_OBJECT
public:
    TimeView(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~TimeView(void);

    void setModel(PhysicalModel*);

public slots:
    void updateModel();
    void modelChanged();
    void setCurrentTime(double);
    void change_Time();

private:
    PhysicalModel *model;
    TimeParameters last;

    QLineEdit *leTime;
    QLineEdit *leTmin;
    QLineEdit *leTmax;
    QLineEdit *leHTime;
};
