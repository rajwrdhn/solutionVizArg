/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#include "aspartix.h"
#include <QDebug>
#include "textedit.h"


//HELPME: is this used
//solved
Aspartix::Aspartix(QObject *parent) :
    QObject(parent)
{

}
/**
 * @brief Aspartix::Aspartix
 * @param aspartix_address
 * @param exePath
 * @param arguments
 * @param textview
 * @param parent
 * constructor of aspartix class.
 */
Aspartix::Aspartix (QString aspartix_address,
                    QString exePath, QStringList arguments,
                    TextEdit *textview, QObject *parent )
{
    std::cout << "[Aspartix] attempting to start process\n";
    text = textview;
    text->clear();

    aspartix_d = aspartix_address + "ASPARTIX-D";
    // Create the process
    newProcess = new QProcess(parent);
    newProcess->setReadChannel(QProcess::StandardOutput);
    newProcess->setWorkingDirectory(aspartix_d);
    std::cout << "[Aspartix] Starting Process: " << exePath.toStdString() << std::endl;
    std::cout << "[Aspartix] Starting Process at: " << aspartix_d.toStdString() << std::endl;


    for (int i = 0; i < arguments.size(); i++) {
        std::cout << "Argument #" << i << ": "<< arguments.at(i).toStdString() << std::endl;
    }

    // Start the process with arguments
    newProcess->start(exePath, arguments);

    /* show output */
    connect(newProcess, SIGNAL(readyReadStandardOutput()),this, SLOT(rightMessage()) ); //correct output
    connect(newProcess, SIGNAL(readyReadStandardError()), this, SLOT(wrongMessage()) ); //error

}

// show right message
void Aspartix::rightMessage()
{
    QByteArray strdata = newProcess->readAllStandardOutput();
    text->setTextColor(Qt::black);
    text->append(strdata);
}

// show wrong message
void Aspartix::wrongMessage()
{
    QByteArray strdata = newProcess->readAllStandardError();
    qDebug()<< strdata;
    //text->setTextColor(Qt::red);
    //text->append(strdata);
}
