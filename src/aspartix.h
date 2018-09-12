/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#ifndef ASPARTIX_H
#define ASPARTIX_H

using namespace std;

#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <iostream>
class TextEdit;
/**
 * @brief The Aspartix class is used for the
 * calling of aspartix which is an external
 * script file.
 */
class Aspartix:public QObject
{
    Q_OBJECT
public:
    explicit Aspartix(QObject *parent = 0);
    explicit Aspartix(QString,QString exePath, QStringList arguments, TextEdit *textview, QObject *parent = 0);

public slots:
    void rightMessage();
    void wrongMessage();
protected:
    TextEdit *text;
private:
    QProcess *newProcess;
    QString aspartix_d;
};

#endif // ASPARTIX_H
