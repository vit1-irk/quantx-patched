#include <QTextCodec>
#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

void installEnglishTranslation(QApplication *app)
{
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
    mainWindow.setWindowTitle(mainWindow.tr("Решение уравнения Шредингера в кусочно-постоянном потенциале"));
//    QApplication::setPalette(pal, true);
    mainWindow.show();

    QObject::connect( &mainWindow, SIGNAL(closed()), qApp, SLOT(quit()) );
    
    return app.exec();
}
