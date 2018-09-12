/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#include "guidecircle.h"
#include "graphicswidget.h"

GuideCircle::GuideCircle ( GraphicsWidget *gw, int x0, int y0, int radius ) : graphicsWidget ( gw ){
    //to view the circles
    graphicsWidget->scene()->addItem ( this );
    setFlags(ItemSendsGeometryChanges | ItemIsSelectable | ItemIsMovable);
    arg_x0=x0;
    arg_y0=y0;
    arg_radius=radius;
    setZValue ( 250 );
    circle=true;
}



GuideCircle::GuideCircle ( GraphicsWidget *gw,  int y0, int w) : graphicsWidget ( gw ){
    graphicsWidget->scene()->addItem ( this );
    arg_y0=y0;
    width= w;
    setZValue ( 250 );
    circle=false;
}


/** Returns the bounding rectangle of the background circle*/
QRectF GuideCircle::boundingRect() const {
    if (circle) {
     return QRectF ( arg_x0 - arg_radius-1, arg_y0 - arg_radius-1, arg_x0 + 2 * arg_radius + 1, arg_y0 + 2* arg_radius +1 );
    }
    else  {
     return QRectF ( 1, arg_y0 -1,  width, arg_y0 + 1 );
    }
}

//paint the guide circle or the line
void GuideCircle::paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * ){
    Q_UNUSED(option);
    painter->setPen ( QPen ( QColor ( "red" ), 1, Qt::DotLine ) );
    if (circle) {
        painter->drawEllipse ( QPoint(arg_x0, arg_y0), arg_radius, arg_radius );
    }
    else {
        painter->drawLine ( 10 , arg_y0, width-10 , arg_y0);
    }
}


//clear the guides
void GuideCircle::die (){
    this->prepareGeometryChange();
    this->hide();
    this->update();
    graphicsWidget->scene()->removeItem(this);
    this->update();
}
