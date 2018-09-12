/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#include <QApplication>		//core Qt functionality
#include "mainwindow.h"		//main application window

using namespace std;


int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);
    QApplication app(argc, argv);
    MainWindow *solvizArg = new MainWindow;
    solvizArg->show();
    return app.exec();
}


