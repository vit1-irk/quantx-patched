#include <QPainter>
#include <QLinearGradient>
#include <QPaintEvent>
#include "MySplitter.h"

class MySplitterHandle : public QSplitterHandle
{
public:
    MySplitterHandle(Qt::Orientation orientation, QSplitter *parent = 0);
    void paintEvent(QPaintEvent *event); 
private:
    QColor c0, c1;
};

MySplitterHandle::MySplitterHandle(Qt::Orientation orientation, QSplitter *parent)
: QSplitterHandle(orientation,parent), c0(Qt::gray), c1(Qt::gray)
{
}

void MySplitterHandle::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QLinearGradient gradient;
    gradient.setColorAt(0,c0);
    gradient.setColorAt(1,c1);
    if (orientation() == Qt::Vertical)
    {
        gradient.setStart(rect().left(), rect().height()/2);
        gradient.setFinalStop(rect().right(), rect().height()/2);
    } else {
        gradient.setStart(rect().width()/2, rect().top());
        gradient.setFinalStop(rect().width()/2, rect().bottom());
    }
    painter.fillRect(event->rect(), QBrush(gradient));
}

MySplitter::MySplitter(Qt::Orientation orientation, QWidget *parent)
: QSplitter(orientation,parent)
{
}

QSplitterHandle *MySplitter::createHandle()
{
    return new MySplitterHandle( orientation(), this );
}
