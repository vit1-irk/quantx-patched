#pragma once
#include <QDialog>
#include <QLabel>
#include "Plotter.h"

class PlotterDialog : public QWidget
//class PlotterDialog : public QDialog
{
    Q_OBJECT

public:
    PlotterDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    Plotter *plotter() const { return _plotter; }

protected slots:
    void processInfoMouseMovedTo(QPointF);
private:
    Plotter *_plotter;
    QLabel *label;
};
