/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#ifndef GUIDECIRCLE_H
#define GUIDECIRCLE_H
#include <QGraphicsItem>
#include <QObject>


class GraphicsWidget;

static const int TypeGuide = QGraphicsItem::UserType+6;

class GuideCircle : public QObject, public  QGraphicsItem {
    Q_OBJECT
    Q_INTERFACES (QGraphicsItem)

public:
    GuideCircle(GraphicsWidget *, int, int, int );
    GuideCircle(GraphicsWidget *, int, int );
    enum { Type = UserType + 6 };
    int type() const { return Type; }
    void die();


protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    GraphicsWidget *graphicsWidget;
    int arg_x0, arg_y0, arg_radius, width;
    bool circle;

};

#endif // GUIDECIRCLE_H
