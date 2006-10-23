#include <qapplication.h>
#include <qtextcodec.h>
#include "Peru.h"

namespace PeruGlobals {
  extern QString execLocation = QString::null;
};

using namespace PeruGlobals;

int main( int argc, char** argv )
{
    QString argv_string = QString(argv[0]);
    if( argv_string[0] == QChar('/') ||
	argv_string[0] == QChar('~') )
      execLocation = argv_string;
    else
      execLocation = QString(getenv("PWD")) + QChar('/') + QString(argv[0]);

    execLocation = execLocation.left( execLocation.findRev(QChar('/')) + 1 );
  
    QApplication a( argc, argv );

    QString locale = QTextCodec::locale();
    locale = locale.left( locale.find(QChar('.')) );

    fprintf(stderr, "Starting Peru, locale='%s'\n", locale.latin1() );
    fprintf(stderr, "ExecLocation is '%s'\n", execLocation.latin1() );
    
    bool ok = false;

    // translation file for Qt
    QTranslator qt( 0 );
    ok = qt.load( QString( "qt_" ) + locale, execLocation );
    a.installTranslator( &qt );
    fprintf(stderr, "QT  Translation = %i\n", ok );

    // translation file for application strings
    QTranslator myapp( 0 );
    ok = myapp.load( QString( "peru_" ) + locale, execLocation );
    a.installTranslator( &myapp );
    fprintf(stderr, "App Translation = %i\n", ok );

    Peru* w = new Peru;
    w->show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    
    int ret = a.exec();

    delete w;

    return ret;
}
