/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#ifndef GRAPHICSWIDGET_H
#define GRAPHICSWIDGET_H

using namespace std;

#include <QGraphicsView>
#include <QList>

class MainWindow;
class Argument;
class Attack;
class TextEdit;


typedef QHash <QString, Attack*> H_StrToAttack;
typedef QHash <long int, Argument*> H_NumToArgument;
typedef QHash <QString, Argument*> forFindArg;
typedef QHash <QString, int> forBestArgument;
typedef QHash <QString, int> argSol;
typedef QHash <Argument*, QPointF> argPositon;
/**
 * @brief The GraphicsWidget class consists of the main graphics widget
 * consisting of the graph scene, the visualizing techniques
 * and the arguments details.
 */
class GraphicsWidget : public QGraphicsView {
    Q_OBJECT
public:
    GraphicsWidget(QGraphicsScene*, TextEdit *text, MainWindow* parent);
    ~GraphicsWidget();
    void clear();
    QStringList allArgumentList;

    Argument* hasArgument(QString text);
    //	Argument* hasArgument(int number);
    bool setMarkedArgument(QString text);
    TextEdit *tex;
    QList<QGraphicsItem *> selectedItems();
    QStringList semantics;
    void selectAll();
    void selectNone();
    forFindArg ArgFind;
    argSol argSolSize;
    argPositon argInitPos;
    void removeItem(Attack*);
    void removeItem(Argument*);
    //void removeItem(ArgumentNumber*);
    void insertIntoargSolSize(QString arg, int argsized);
    void ArgumentMoved(int, int, int);

    void setInitArgumentColor(QString);
    void setInitLinkColor(QString);
    void setInitArgumentSize(int);
    void setInitNumberDistance(int);
    void setInitLabelDistance(int);

    void setAllItemsVisibility(int, bool);

    void removeAllItems(int);
    Argument *hasArgumentNumber(int n);

    void setAllArgumentColor();
    void setSolutionColordial(QColor color, QStringList list);
    QString randomSelectShapeArg();
    void setArgumentShape(QString);
    void setAllArgumentSizeAndShape(int size, QString shape);
    void layoutCircularForSolution(double x0, double y0, double maxRadius);
    void layoutLinearPosition(double x0,double y0);
    void layoutLinearPositionRand(double x0, double y0);
    void arrangeSizeOfArg();
    void setAllArgumentsShape();

protected:
    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent ( QMouseEvent * e );
    void mousePressEvent ( QMouseEvent * e );
    void mouseReleaseEvent(QMouseEvent * e );
    //void resizeEvent( QResizeEvent *e );
    void paintEvent ( QPaintEvent * event );

public slots:
    void addGuideCircle( int x0, int y0, int radius);
    void addGuideHLine(int y0);

    void drawArgument(QString argName, int i, int size, QString aColor,
                    QPointF p,
                    QString ArgumentShape, bool upload);
    void eraseArgument(long int doomedJim);
    void setArgumentVisibility(long int, bool );	//Called from Graph via MW
    bool setArgumentColor(long int, QString);
    void setSolutionColor(QString , QStringList);
    void openArgumentContextMenu();
    void ArgumentClicked(Argument *);
    void moveArgument(int, qreal, qreal);	//Called from Graph
    bool setArgumentSize(long int, int size=0);
    void setAllArgumentSize();
    void setArgumentSizeForSolutions(QString, double num);
    void drawAttack(QString, QString, int, int, float, bool, bool, QString, bool, bool );
    void eraseAttack(int, int);
    void setAttackVisibility (int relation, int, int, bool);
    void setAttackColor(long int, long int, QString);
    void AttackClicked(Attack *);
    void openAttackContextMenu();

    void changeZoom(const int value);
    void startAttack(Argument *argument);
    void drawAttackReciprocal(QString,QString,int, int,bool);
    void unmakeAttackReciprocal(int, int);
    void setArgColorSol();


    void zoomIn();
    void zoomOut();

    void moveArgumentX(int number, qreal x);
    void clearGuides();
    void rot(int angle);
    void setInitPos();
    void scaleView(qreal scaleFactor);
    void setArgumentColor(QString arg);
signals:
    void userDoubleClicked(int, QPointF);
    void userMiddleClicked(QString, QString,int,int, float);
    void openArgumentMenu();
    void openAttackMenu();
    void openContextMenu(const QPointF p);
    void updateArgumentCoords(int, int, int);
    void selectedArgument(Argument *);
    void selectedAttack(Attack *);
    void zoomChanged(int);



private:

    H_NumToArgument ArgumentHash;	//This is used in drawAttack() method
    H_StrToAttack AttacksHash; // helper hash to easily find Attacks

    int arg_curRelation;
    int timerId,  layoutType, arg_ArgumentSize, arg_numberDistance, arg_labelDistance;
    double arg_currentScaleFactor;
    int zoomIndex, markedArgumentOrigSize,markedAttackSourceOrigSize, markedAttackTargetOrigSize;
    QString arg_ArgumentLabel, arg_numberColor, arg_ArgumentColor, arg_labelColor, arg_linkColor;
    int arg_currentRotationAngle;
    bool secondDoubleClick, dynamicMovement, markedArgumentExist, markedAttackExist;
    QGraphicsItem *moving;
    QPointF startPoint, endPoint;
    Argument *firstArgument, *secondArgument, *markedArgument1, *markedAttackSource;
    Argument *markedAttackTarget, *tempArgument ;

};

#endif
