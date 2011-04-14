#pragma once
#include <QDialog>
#include <QLineEdit>
#include "PhysicalModel.h"

class LevelNumber : public QDialog
{
    Q_OBJECT
public:
    LevelNumber(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~LevelNumber(void);

    void setModel(PhysicalModel*);

public slots:
    void updateModel();
    void modelChanged();

private:
    PhysicalModel *model;
    LevelNumberParameters last;

    QLineEdit *leHn;
    QLineEdit *leNmin;
    QLineEdit *leNmax;
};
