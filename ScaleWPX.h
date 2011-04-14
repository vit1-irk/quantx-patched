#pragma once
#include <QDialog>
#include <QLineEdit>
#include "PhysicalModel.h"

class ScaleWPX : public QDialog
{
    Q_OBJECT
public:
    ScaleWPX(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~ScaleWPX(void);

    void setModel(PhysicalModel*);

public slots:
    void updateModel();
    void modelChanged();

private:
    PhysicalModel *model;
    ScaleWPXParameters last;

    QLineEdit *lePsimin;
    QLineEdit *lePsimax;
    QLineEdit *leXmin;
    QLineEdit *leXmax;
    QLineEdit *leHx;
};
