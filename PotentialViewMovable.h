#pragma once
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>
#include "PhysicalModel.h"

class DraggableLine;

class PotentialViewMovable : public QWidget
{
    Q_OBJECT
public:
    PotentialViewMovable(QWidget * parent = 0, Qt::WindowFlags f = 0);
    ~PotentialViewMovable(void);

    void setModel(PhysicalModel*);

public slots:
    void modelChanged();

protected slots:
    void updateModel();

private:
    PhysicalModel *model;

    QList<DraggableLine*> lines;
    QGraphicsScene *scene;
    QGraphicsView  *view;
};
