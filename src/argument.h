/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/

#ifndef Argument_H
#define Argument_H

using namespace std;

#include <QGraphicsItem>
#include <QObject>
#include <QPolygon>

class GraphicsWidget;
class QGraphicsSceneMouseEvent;
class Attack;


static const int TypeArgument = QGraphicsItem::UserType+1;

/**
*  This is actually a container-class.
*  Contains the graphical objects called Arguments or nodes.
*  which are displayed as circles/squares/polygons/squares..etc on the canvas.
*  Each Argument "knows" the others with which it is connected.
*/

class Argument :  public QObject,  public QGraphicsItem {

    Q_OBJECT

    Q_INTERFACES (QGraphicsItem)

public:
    Argument(GraphicsWidget*, QString argName, int num, int size, QString col, QString shape, int, int, QPointF p) ;
    ~Argument();

    enum { Type = UserType + 1 };
    int type() const { return Type; }

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    long int argumentNumbe() {return arg_num;}
    QString argumentName() {return arg_name;}

    void setSize(double);
    int size();

    void setShape (const QString);
    QString ArgumentShape() {return arg_shape;}

    void setColor(QString str);
    void setColor(QColor color);
    QString color ();

    void addInLink( Attack *att ) ;
    void deleteInLink(Attack*);

    void addOutLink( Attack *att ) ;
    void deleteOutLink(Attack*);

    void toggleAntialiasing(bool);



    void clear();
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
signals: 
    void ArgumentClicked(Argument *);
    void openArgumentContextMenu();
    void startAttack(Argument *);
    void adjustOutAttack();
    void adjustInAttack();
    void removeOutAttack();
    void removeInAttack();
private:
    GraphicsWidget *graphicsWidget;
    QPainterPath *arg_path;
    QPointF newPos;
    QPolygon *arg_poly_t, *arg_poly_d;
    int  arg_size, arg_nd, arg_ld;
    QString arg_name;
    long int arg_num;
    QString  arg_shape,  arg_col_str, arg_labelIn;
    QColor arg_col;
    /**Lists of elements attached to this Argument */
    list<Attack*> inAttackList, outAttackList;

};

#endif
