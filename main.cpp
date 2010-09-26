#include <qtextcodec.h>
#include "mainwindow.h"
#include <qapplication.h>

int
main(int argc, char **argv)
{
    QApplication app( argc, argv );
//    QPalette pal(Qt::green.light(200), Qt::blue.light(200));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("CP1251"));
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));//!!!!!!!!!
 
    MainWindow mainWindow;

    mainWindow.resize(mainWindow.sizeHint());
    QFont font( "Serif", 10, QFont::DemiBold );
    mainWindow.setFont(font);
    mainWindow.setWindowTitle("Solution of the Schroedinger equation for the step-wise potential");
//    QApplication::setPalette(pal, true);
    mainWindow.show();

    QObject::connect( &mainWindow, SIGNAL(closed()), qApp, SLOT(quit()) );
    
    return app.exec();
}
