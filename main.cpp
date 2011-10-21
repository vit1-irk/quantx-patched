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

#include <QTextCodec>
#include "MainWindow.h"
#include <QApplication>
#include <QTranslator>
#include <QTime>
#include <QFont>

void installEnglishTranslation(QApplication *app)
{
    Q_UNUSED(app);
    QTranslator *trr = new QTranslator;
    if (trr->load("kvant_en")) goto itr;
    if (trr->load("kvant_en","..")) goto itr;
    delete trr;
    return;
itr:
    app->installTranslator(trr);
}

int
main(int argc, char **argv)
{
    QApplication app( argc, argv );

    QTextCodec::setCodecForTr(QTextCodec::codecForName("Windows-1251"));

    //installEnglishTranslation(&app); //optional

//    QPalette pal(Qt::green.light(200), Qt::blue.light(200));
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));//!!!!!!!!!

    MainWindow mainWindow;

    mainWindow.resize(mainWindow.sizeHint());

    QFont font( "Courier", 12, QFont::DemiBold );
//    QFont font( "Serif", 10, QFont::DemiBold );
    mainWindow.setFont(font);
//    mainWindow.setPointSize(10);
    //mainWindow.setWindowTitle("Solution of the Schroedinger equation for the step-wise potential");
    mainWindow.setWindowTitle(mainWindow.tr("Решение уравнения Шредингера c кусочно-постоянным потенциалом"));
//    QApplication::setPalette(pal, true);
    mainWindow.show();

    QObject::connect( &mainWindow, SIGNAL(closed()), qApp, SLOT(quit()) );

    return app.exec();
}
