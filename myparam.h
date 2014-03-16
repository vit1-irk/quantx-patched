/*
 *  Copyright (C) 2011 Olga Tkachenko
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA
 */

#ifndef myparam_h_included
#define myparam_h_included

#include <QLineEdit>

class MyParamD : public QObject
{
    Q_OBJECT
public:
    MyParamD(double _v = 0) : v(_v), ledit(0) {}
    MyParamD& operator=(const MyParamD&);
    double& operator=(double d) { return v = d; }
    double& operator+=(double d) { return v += d; }
    operator double() const { return v; }
    void setDisplay(const QString& label, const QString& labelTip, QWidget *parent);
    void setDisplay(const QString& label, const QString& labelTip, QLayout *parent);
//    void setDisplay(const QString& label, QWidget *parent);
//    void setDisplay(const QString& label, QLayout *parent);
    ~MyParamD() { if (ledit) delete ledit; }

public slots:
    void updateDisplay();
    void textToValue();

signals:
    void valueChanged();

private:
    QLineEdit *ledit;
    double v;
};

class MyParamI : public QObject
{
    Q_OBJECT
public:
    MyParamI(int _v = 0) : v(_v), ledit(0) {}
    int& operator=(int d) { return v = d; }
    int& operator+=(int d) { return v += d; }
    operator int() const { return v; }
    void setDisplay(const QString& label, const QString& labelTip, QWidget *parent);
    void setDisplay(const QString& label, const QString& labelTip, QLayout *parent);
//    void setDisplay(const QString& label, QWidget *parent);
//    void setDisplay(const QString& label, QLayout *parent);
    ~MyParamI() { if (ledit) delete ledit; }

public slots:
    void updateDisplay();
    void textToValue();

signals:
    void valueChanged();

private:
    QLineEdit *ledit;
    int v;
};
/*
class MyParamUxN : public QObject
{
    Q_OBJECT
public:
    MyParamUxN(int _v = 0) : v(_v), ledit(0) {}
    int& operator=(int d) { return v = d; }
    int& operator+=(int d) { return v += d; }
    operator int() const { return v; }
    void setDisplay(const QString& label, QWidget *parent);
    void setDisplay(const QString& label, QLayout *parent);
    ~MyParamUxN() { if (ledit) delete ledit; }

public slots:
    void updateDisplay();
    void textToValue();
    void showU();

signals:
    void valueChanged();

private:
    QLineEdit *ledit;
    int v;
};
*/
#endif
