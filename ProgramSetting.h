#pragma once
#include <QDialog>
#include <QLineEdit>
#include "PhysicalModel.h"

class ProgramSetting : public QDialog
{
    Q_OBJECT
public:
    ProgramSetting(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~ProgramSetting(void);

    void setModel(PhysicalModel*);

public slots:
    void updateModel();
    void modelChanged();

private:
    PhysicalModel *model;
    SettingParameters last;

    QLineEdit *leLineWidth;
};
