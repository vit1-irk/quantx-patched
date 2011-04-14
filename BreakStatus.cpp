#include <QApplication>
#include "BreakStatus.h"

static volatile int localBreakStatus = 0;

void BreakStatus::set1()
{
    localBreakStatus = 1;
}

bool BreakStatus::onButton(QToolButton *button)
//bool BreakStatus::onButton(QPushButton *button)
{
    localBreakStatus = 0;
    return connect(button,SIGNAL(clicked()),this,SLOT(set1()),Qt::DirectConnection);
}

bool BreakStatus::noButton(QToolButton *button)
//bool BreakStatus::noButton(QPushButton *button)
{
    return disconnect(button,SIGNAL(clicked()),this,SLOT(set1()));
}

BreakStatus breakStatus;

int getBreakStatus(int newStatus)
{
    qApp->processEvents(QEventLoop::AllEvents);
    int res = localBreakStatus;
    localBreakStatus = newStatus;
    return res;
}

void setBreakStatus(int newStatus)
{
    localBreakStatus = newStatus;
}
