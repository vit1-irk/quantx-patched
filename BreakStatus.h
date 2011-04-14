#pragma once

#include <QObject>
#include <QToolButton>
//#include <QPushButton>

class BreakStatus : public QObject
{
    Q_OBJECT
public:
    //! Attach the break status to click of \a button
    bool onButton(QToolButton *button);
//    bool onButton(QPushButton *button);

    //! Detach the break status from \a button
    bool noButton(QToolButton *button);
//    bool noButton(QPushButton *button);

private slots:
    //! Helper slot
    void set1(void);
};

extern BreakStatus breakStatus;

int getBreakStatus(int newStatus = 0);
void setBreakStatus(int newStatus);
