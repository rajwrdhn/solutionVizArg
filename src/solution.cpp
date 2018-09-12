/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#include "solution.h"
#include "textedit.h"
#include <QDebug>
#include <QPointF>
#include "graphicswidget.h"
#include <QHash>

/**
 * @brief Solution::Solution
 * @param text
 * @param view
 * Contains the MapReduce and calculation of the total number of extensions.
 */
Solution::Solution(TextEdit *text, GraphicsWidget *view)
{
    en_text = text;
    en_view = view;
}

Solution::~Solution()
{
    clear();
}

/**
 * @brief Solution::getNextSolutionSet The iterator method for the next button.
 * @param i
 * @param argList
 * HelpME: AM I using this? solved used by solutiondialogiter.
 */
QStringList Solution::getNextSolutionSet(QString argList)
{
    //int n = argList.length();
    qDebug() << argList << "::arguments list in solution";
    argList = argList.simplified();
    QStringList tr1 = argList.split(",", QString::SkipEmptyParts);
    qDebug() << tr1 << "::arguments in the solution";
    return tr1;
}


/**
 * @brief Solution::getPresentSolutionset The method returns true,
 * if an argument is present in a particular solution set.
 * @param argList
 * @param argName
 * @return used by mainwindow::getEnumerations
 */
bool Solution::getPresentSolutionset(QString argList, QString argName)
{

    QStringList tr1 = argList.split(",", QString::SkipEmptyParts);

    if (tr1.contains(argName)) {
        return true;
    } else {
        return false;
    }

}

/**
 * @brief Solution::calculateMap
 * @param afSolutionstr
 * the Map and reduce as a concurrent
 * map reduce mapped in qmultihash
 */
void Solution::calculateMapReduce(QString afSolutionstr)
{
    qDebug() << "Solution::calculateMapReduce()";
    //for empty set and no solution
    if ( afSolutionstr.contains("[[]]") ) {
        theMap.insert(afSolutionstr, "{}");
        return;
    } else if (afSolutionstr.contains("[]") && !afSolutionstr.contains("[[]]") && !afSolutionstr.contains("],[")) {
        qDebug() << "No solution !!" ;
        return;
    }

    QString strSolution = afSolutionstr;
    QStringList strListSol = strSolution.split(",",QString::SkipEmptyParts);


    i_extension_number = 0;

    //string parsing for the solution
    foreach (QString str, strListSol) {
        //one solution with one argument
        if (str.contains("[[") && str.contains("]]")  ){
            qDebug() << "solution string contains [[ and ]]" << str ;
            str.remove("[[");
            str.remove("]]");
            str = str.simplified();
            qDebug() << "solution string contains [[ and ]]" <<str ;
            theMap.insert(str, "extension:" + QString::number(i_extension_number));
        } else if (str.contains("[[") && !str.contains("]]") && !str.contains("]")) { //first starting argument
            qDebug() << "solution string contains [[ and does not contain ]] and ]." << str;
            str.remove("[[");
            str = str.simplified();
            qDebug() << "solution string contains [[ and does not contain ]] and ]." << str;
            theMap.insert(str, "extension:" + QString::number(i_extension_number));
        } else if (str.contains("]]") && !str.contains("[[") && !str.contains("[") ) {//last argument in an extension
            qDebug() << "solution string contains ]] and does not contain [[ and [." << str ;
            str.remove("]]");
            str = str.simplified();
            qDebug() << "solution string contains ]] and does not contain [[ and [." << str ;
            theMap.insert(str, "extension:" + QString::number(i_extension_number));
        } else if (str.contains("]") && !str.contains("[") && !str.contains("]]") && !str.contains("[[")) {
            //last argument of an extension
            qDebug() << "solution string contains ] and does not contain [ and ]]";
            str.remove("]");
            str = str.simplified();
            theMap.insert(str,"extension:" + QString::number(i_extension_number));
        } else if (str.contains("[") && str.contains("]]") && !str.contains("[[") ) {
            //last extension has only one argument but it is not the only extension.
            qDebug() << "solution string contains ]] and [ does not contain [[." << str;
            ++i_extension_number;
            str.remove("[");
            str.remove("]]");
            str = str.simplified();
            theMap.insert(str,"extension:" + QString::number(i_extension_number));
        } else if (str.contains("[") && str.contains("]") && !str.contains("[[") && !str.contains("]]")) {
            //one of the extension contains only one argument but not the first or the last extension.
            qDebug() << "solution string contains "<< str;
            ++i_extension_number;
            str = str.simplified();
            theMap.insert("{}", "extension:" + QString::number(i_extension_number));
        } else if (str.contains("[") && !str.contains("]") && !str.contains("[[") && !str.contains("]]")) {
            //first argument of a solution
            qDebug() << "solution string contain." <<  str;
            str.remove("[");
            ++i_extension_number;
            str = str.simplified();
            theMap.insert(str, "extension:" + QString::number(i_extension_number));
        } else if (str =="[]]") {
            qDebug() << "solution string contains []]." << str;
            ++i_extension_number;
            theMap.insert("{}", "extension:" + QString::number(i_extension_number));
        } else {
            qDebug () << str ;
            str = str.simplified();
            theMap.insert(str,"extension:" + QString::number(i_extension_number));
        }
    }
    emit signalsetSizeOfArgument();
}

//Sceptical Acceptance Color
void Solution::setScepticalDataCol() {
    qDebug() << "Solution::Sceptical";
    foreach (QString arg, en_view->allArgumentList) {
        if(this->numberOfExtensions(en_text->toPlainText()) == theMap.count(arg)) {
            en_view->setArgumentColor(arg);
        } else {
            qDebug() << "color of non sceptical";
        }
    }
}

/**
 * @brief Solution::calculateShapeRandom
 * Calculates the shape and size , shape is calculated radomly.
 * called from MW visualization.
 * shape to square box
 */
void Solution::calculateShapeRandom()
{
    qDebug() << "Solution::calculateShapeRandom()";
    foreach (QString arg, en_view->allArgumentList) {
        if(theMap.contains(arg)) {
            en_view->setArgumentShape(arg);
        } else {
            en_view->setArgumentSizeForSolutions(arg,13);
        }
    }    
}

/**
 * @brief Solution::calculateColor
 * Calculates the color of the arguments present in the solution
 * called from MW visualization.
 * color to blue
 */
void Solution::calculateColor()
{
    qDebug() << "Solution::calculateShapeRandom()";
    foreach (QString arg, en_view->allArgumentList) {
        if(theMap.contains(arg)) {
            en_view->setArgumentColor(arg);
        } else {
            qDebug() << arg <<" Not Present in the solution!!";
        }
    }
}

/**
 * @brief Solution::calculateSize
 * Calculates the new size.
 * called from MW visualization.
 */
void Solution::calculateSize()
{
    qDebug() << "Solution::calculateSize()";
    foreach (QString arg, en_view->allArgumentList) {
        if(theMap.contains(arg)) {
            QList<QString> values = theMap.values(arg);
            qDebug() << theMap.values(arg);
            double size = 13 +  (values.size())/10 ;
            en_view->setArgumentSizeForSolutions(arg,size);
        } else {
            en_view->setArgumentSizeForSolutions(arg,12);
        }
    }
}

/**
 * @brief Solution::normalizeSize
 * revertss to the old size.
 * called from mapReduce.
 */
void Solution::normalizeSize()
{
    qDebug() << "Solution::normalizeSize() for all arguments.";
    foreach (QString arg, en_view->allArgumentList) {

        en_view->setAllArgumentSizeAndShape(13, "circle");

            if (!theMap.value(arg).isEmpty()) {
                theMap.remove(arg);
            } else {
                qDebug() << "Map Reduce Empty !!";
            }
    }
    theMap.clear();
}

/**
 * @brief Solution::numberOfExtensions
 * @param str
 * @return Calculate the total Number of Extensions
 */
int Solution::numberOfExtensions(QString str)
{
    qDebug() << "Solution::numberOfExtensions()";
    if (str.contains("],[")) {
        QStringList strList = str.split("],[");
        int len = strList.length();
        return len;
    } else if ( str.contains("[[]]")) {
        return 1;
    } else if ( str.contains("[]")  && !str.contains("[[]]")) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * @brief Solution::clear
 * clears for new creation of argumentation framework.
 */
void Solution::clear() {
    qDebug() << "Solution::clear()";
    theMap.clear();
    en_text->clear();
    en_view->clear();
}


//Number of extensions an argument is present in.
int Solution::sizeOfArgInSolution(QString arg) {
    int size = theMap.count(arg);
    return size;
}

//set size for external GW, only for arrangement of arguments not incresing the size.
//called from MW button.
void Solution::setSizeOfArgument() {
    foreach (QString arg, en_view->allArgumentList) {
        int size = sizeOfArgInSolution(arg);
        en_view->insertIntoargSolSize(arg,size);
    }
}
