#pragma once
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>
#include "model.h"

class DraggableLine;

class PotentialViewMovable : public QWidget
{
    Q_OBJECT
public:
    PotentialViewMovable(QWidget * parent = 0, Qt::WindowFlags f = 0);
    ~PotentialViewMovable(void);

    void setModel(Model*);

public slots:
    void modelChanged();

protected slots:
    void updateModel();

private:
    Model *model;

    QList<DraggableLine*> lines;
    QGraphicsScene *scene;
    QGraphicsView  *view;
};
