#pragma once
#include <QDialog>
#include <QLineEdit>
#include "PhysicalModel.h"

class PotentialViewMultiWell : public QDialog
{
    Q_OBJECT
public:
    PotentialViewMultiWell(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~PotentialViewMultiWell(void);

    void setModel(PhysicalModel*);

public slots:
    void updateModel();
    void modelChanged();

private:
    PhysicalModel *model;
    UAsMW last;

    QLineEdit *leNumberOfWells;
//    QLineEdit *leUBias;
    QLineEdit *leWidthA;
    QLineEdit *leWidthB;
    QLineEdit *leHeightA;
    QLineEdit *leHeightB;
};
