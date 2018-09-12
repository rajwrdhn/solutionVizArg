/********************************************************************************
** Form generated from reading UI file 'solutiondialogiter.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SOLUTIONDIALOGITER_H
#define UI_SOLUTIONDIALOGITER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolButton>

QT_BEGIN_NAMESPACE

class Ui_SolutionDialog2
{
public:
    QPushButton *pushButton;
    QTextEdit *textEdit;
    QToolButton *toolButton;
    QLabel *label;

    void setupUi(QDialog *SolutionDialog2)
    {
        if (SolutionDialog2->objectName().isEmpty())
            SolutionDialog2->setObjectName(QStringLiteral("SolutionDialog2"));
        SolutionDialog2->resize(383, 238);
        pushButton = new QPushButton(SolutionDialog2);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(130, 190, 99, 27));
        textEdit = new QTextEdit(SolutionDialog2);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setGeometry(QRect(60, 100, 231, 78));
        toolButton = new QToolButton(SolutionDialog2);
        toolButton->setObjectName(QStringLiteral("toolButton"));
        toolButton->setGeometry(QRect(290, 100, 41, 81));
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/colorize.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton->setIcon(icon);
        label = new QLabel(SolutionDialog2);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(0, 0, 371, 91));

        retranslateUi(SolutionDialog2);

        QMetaObject::connectSlotsByName(SolutionDialog2);
    } // setupUi

    void retranslateUi(QDialog *SolutionDialog2)
    {
        SolutionDialog2->setWindowTitle(QApplication::translate("SolutionDialog2", "Dialog", 0));
        pushButton->setText(QApplication::translate("SolutionDialog2", "<-->", 0));
        toolButton->setText(QApplication::translate("SolutionDialog2", "...", 0));
        label->setText(QApplication::translate("SolutionDialog2", "<html><head/><body><p align=\"center\">Iterate through each extension and color <br>the arguments in each extension by selecting<br> the color from color tool button </p></body></html>", 0));
    } // retranslateUi

};

namespace Ui {
    class SolutionDialog2: public Ui_SolutionDialog2 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SOLUTIONDIALOGITER_H
