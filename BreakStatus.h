#pragma once

#include <QObject>
#include <QPushButton>

class BreakStatus : public QObject
{
    Q_OBJECT
public:
    //! Attach the break status to click of \a button
    bool onButton(QPushButton *button);

    //! Detach the break status from \a button
    bool noButton(QPushButton *button);

private slots:
    //! Helper slot
    void set1(void);
};

extern BreakStatus breakStatus;

int getBreakStatus(int newStatus = 0);
void setBreakStatus(int newStatus);
