#pragma once
#include <QSplitter>

class MySplitter : public QSplitter
{
public:
     MySplitter(Qt::Orientation orientation, QWidget *parent = 0);

protected:
     QSplitterHandle *createHandle();
};

