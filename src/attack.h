/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#ifndef Attack_H
#define Attack_H

using namespace std;

#include <QGraphicsItem>
#include <QObject>
#include <utility> //declares pair construct


class GraphicsWidget;
class QGraphicsSceneMouseEvent;
class Argument;
class Attackwidth;


static const int TypeAttack= QGraphicsItem::UserType+2;

/**
 * @brief The Attack class is a container class
 * containing the edges or attacks as mentioned in
 * argumentation framework.
 */
class Attack : public QObject, public QGraphicsItem {
	Q_OBJECT
	Q_INTERFACES (QGraphicsItem)

public:
    QString eFrom, eTo;
    int numFrom, numTo;
    Attack(GraphicsWidget *, Argument*, Argument*,
         const Qt::PenStyle &style,
         const float &, const int &, const QString &,
         const bool&, const bool&, const bool &);
    ~Attack();
	enum { Type = UserType + 2 };
	int type() const { return Type; }
    Argument *sourceArgument() const;
    void setSourceArgument(Argument *argument);

    Argument *targetArgument() const;
    void setTargetArgument(Argument *argument);
	
	void setStartOffset(int );
	void setEndOffset(int );
	void removeRefs();

    QString sourceArgumentName();
    QString targetArgumentName();


    int sourceArgumentNumber();
    int targetArgumentNumber();

    void showArrows(bool);
    void toggleAntialiasing(bool);

    void makeReciprocal();
    void unmakeReciprocal();
    bool isReciprocal();

    float width() const;

    QPen pen() const;
    void setStyle( const Qt::PenStyle  &style);
    Qt::PenStyle style() const;

    void setColor( const QString &str) ;
    QString color() const ;
    QString colorToPajek();

	QPainterPath shape() const;
public slots:
	void adjust ();

protected:
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);   
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private:
	GraphicsWidget *graphicsWidget;
    Argument *source, *target;
	QPointF sourcePoint, targetPoint;
    qreal arg_arrowSize, arg_startOffset, arg_endOffset;
    Qt::PenStyle arg_style;
    list<Attackwidth*> widthList;
    QString arg_color;
    float arg_width;
    int size;
	double rad, theta, theta1, theta2;
	qreal angle, line_length, line_dx, line_dy;
    bool arg_Bezier, arg_drawArrows, arg_reciprocal;
};

#endif
