#include <qapplication.h>
#include <qtextcodec.h>
#include "Peru.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );

    QString locale = QTextCodec::locale();
    locale = locale.left( locale.find(QChar('.')) );

    fprintf(stderr, "Starting Peru, locale='%s'\n", locale.latin1() );

     bool ok = false;

    // translation file for Qt
    QTranslator qt( 0 );
    ok = qt.load( QString( "qt_" ) + locale, "." );
    a.installTranslator( &qt );
    fprintf(stderr, "QT  Translation = %i\n", ok );

    // translation file for application strings
    QTranslator myapp( 0 );
    ok = myapp.load( QString( "peru_" ) + locale, "." );
    a.installTranslator( &myapp );
    fprintf(stderr, "App Translation = %i\n", ok );

    Peru *w = new Peru;
    w->show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    
    int ret = a.exec();

    delete w;

    return ret;
}
