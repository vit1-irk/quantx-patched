#pragma once
#include <QDialog>
#include <QLineEdit>
#include "PhysicalModel.h"

class ScaleWPK : public QDialog
{
    Q_OBJECT
public:
    ScaleWPK(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~ScaleWPK(void);

    void setModel(PhysicalModel*);

public slots:
    void updateModel();
    void modelChanged();

private:
    PhysicalModel *model;
    ScaleWPKParameters last;

    QLineEdit *lePsimin;
    QLineEdit *lePsimax;
    QLineEdit *leXmin;
    QLineEdit *leXmax;
    QLineEdit *leHx;
};
