#include <qapplication.h>
#include "Peru.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    Peru *w = new Peru;
    w->show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    
    int ret = a.exec();

    delete w;

    return ret;
}
