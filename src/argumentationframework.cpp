/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#include "argumentationframework.h"
#include "textedit.h"
#include "attack.h"
#include "graphicswidget.h"
#include <QDebug>
#include "mainwindow.h"
#include <QPointF>

ArgumentationFramework::ArgumentationFramework(GraphicsWidget *view, TextEdit *text)
    :gview(view)
{
    frameworkDraw = true;
    aftext = text;
}

/**
 * @brief ArgumentationFramework::~ArgumentationFramework
 * remove kill
 */
void ArgumentationFramework::clear()
{
    mapArgument.clear();
    gview->clear();
    aftext->clear();
}

/**
 * @brief ArgumentationFramework::getAllArguments
 * parsing of all the arguments from the format of apx.
 */
void ArgumentationFramework::getAllArguments()
{
    qDebug() << "AF::getAllArguments on upload!!";
    QString allFile = aftext->toPlainText();

    if (!allFile.contains(".")) {

        qDebug() << "Not Proper Format Need to have a '.' dot!!";
        return;
    }

    QStringList listofArgAtt = allFile.split('.', QString::SkipEmptyParts);
    n = listofArgAtt.size();
    QStringList argumentList;
    for (int i=0; i < n; i++) {
        str = listofArgAtt[i];

        if (str.contains("arg(") && str.contains(")") && !str.contains(QRegExp(QString::fromUtf8("[-`~!@#$%^&*_—+=|:;<>«»,.?/{}\'\"\\\\]")))) {
            str1 = str.section('(',1,1);
            str1.chop(1);
            if (str1.isEmpty() || str1.contains(",")) {
                qDebug() << "Argument Name:: " << str1 << " incorrect format!!";
            } else {
                argumentList << str1;
            }
        } else {
            qDebug() << str << "String Not Considerd, Format Incorrect!!";
        }
    }
    int j = 1;
    foreach (const QString &str, argumentList) {
        emit drawNode(true,str);
        mapArgument.insert(str,j);
        j++;
    }

}


/**
 * @brief ArgumentationFramework::getAllAttacks
 * parsing of all the attacks from the format of apx.
 */
void ArgumentationFramework::getAllAttacks()
{
    qDebug() << "AF::getALLAttacks from Uploadof apx!!";
    QString allFile = aftext->toPlainText();
    QStringList listofArgAtt = allFile.split('.', QString::SkipEmptyParts);
    n = listofArgAtt.size();

    QStringList attackList;

    for (int i=0; i < n; i++) {
        str = listofArgAtt[i];
        if (str.contains("att(") && !str.contains(QRegExp(QString::fromUtf8("[-`~!@#$%^&*_—+=|:;<>«».?/{}\'\"\\\\]"))) && str.contains(")")) {
            str1 = str.section('(',1,1);
            str1.chop(1);
            attackList << str1;
        }
    }

    QStringList attackTuple;
    QString s1 ;
    QString s2 ;

    foreach (const QString &str, attackList) {
        attackTuple = str.split(',', QString::SkipEmptyParts);

        //for (int i =0; i < 2; i++) {
            s1 = attackTuple.value(0);
            s2 = attackTuple.value(1);
            if ( s1.isEmpty() || s2.isEmpty()) {
                qDebug() << "attack Not proper format "<< s1 <<" and " << s2;
                return;
            }
        //}

        if(gview->allArgumentList.contains(s1) && gview->allArgumentList.contains(s2)) {
                emit drawEdge(s1 ,s2 ,mapArgument[s1],mapArgument[s2]);
        } else {
            qDebug() << "Attack Not Possible between " << s1 << " and " << s2 ;
            return;
        }
    }
}


/**
 * @brief ArgumentationFramework::~ArgumentationFramework
 */
ArgumentationFramework::~ArgumentationFramework() {

}
