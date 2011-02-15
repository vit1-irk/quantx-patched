//#pragma once
#include <QDialog>
#include <QLineEdit>
#include "PhysicalModel.h"

class WPparametersM : public QDialog
{
    Q_OBJECT
public:
    WPparametersM(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~WPparametersM(void);

    void setModel(PhysicalModel*);

public slots:
    void updateModel();
    void modelChanged();

private:
    PhysicalModel *model;
    EmWP last;

    QLineEdit *leHn;
    QLineEdit *leNmin;
    QLineEdit *leNmax;
};
