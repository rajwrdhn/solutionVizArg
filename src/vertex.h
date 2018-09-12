/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#ifndef VERTEX_H
#define VERTEX_H

using namespace std;

#include <QObject>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QList>
#include <QPointF>
#include <map>

using namespace std;

class QPointF;
class Graph;


typedef QHash<int,QString> H_IntToStr;
typedef QList<int> ilist;
typedef QPair <float, bool> pair_f_b;
typedef QPair <int, pair_f_b > rel_w_bool;
typedef QHash < int, rel_w_bool > H_Attacks;
typedef QHash <QString, int> H_StrToInt;

class Vertex : public QObject{
    Q_OBJECT

public:

    Vertex(Graph* parent,
           const long int &name,
           const int &val,
           const int &size,
           const QString &color,
           const QPointF &p,
           const QString &shape);

    Vertex(const long int &name);

    ~Vertex();

    long int name() const { return vert_name; }

    void setName (const long int &name) { vert_name=name; }

    void setEnabled (const bool &flag ) { vert_enabled=flag; }
    bool isEnabled () const { return vert_enabled; }

    void changeRelation(int) ;

    void addAttackTo (const long int &v2, const float &weight);
    void addAttackFrom(const long int &v1, const float &weight);

    void changeOutAttackWeight (long int target, float weight);

    void setOutAttackEnabled (long int, bool);

    void removeAttackTo (long int target);
    void removeAttackFrom(long int source);

    QHash<int,float>* returnEnabledOutAttacks();
    QHash<int,float>* returnReciprocalAttacks();

    long int outAttacks();
    long int outAttacksConst() const ;

    long int inAttacks();
    long int inAttacksConst() const ;


    long int outDegreeConst();
    long int inDegree();
    long int inDegreeConst();
    long int localDegree();

    /* sets eccentricity */
    void setEccentricity (float c){ vert_Eccentricity=c;}
    float eccentricity() { return vert_Eccentricity;}

    /* Returns true if there is a reciprocal link from this vertex */
    bool isReciprocalLinked() { return vert_reciprocalLinked;}
    void setReciprocalLinked(bool reciprocal) { vert_reciprocalLinked=reciprocal;}

    /* Returns true if there is an outLink from this vertex */
    bool isOutLinked() { return (outAttacks() > 0) ? true:false;}
    /* Returns the weight of the link to vertex V, otherwise zero*/
    float hasAttackTo(long int V);

    /* Returns true if there is an outLink from this vertex */
    bool isInLinked() { return  (inAttacks() > 0) ? true:false;}
    float hasAttackFrom (long int v);

    bool isIsolated() { return !(isOutLinked() | isInLinked()) ; }
    void setIsolated(bool isolated) {vert_isolated = isolated; }

    void filterAttacksByWeight(float vert_threshold, bool overThreshold);
    //	void filterAttacksByColor(float vert_threshold, bool overThreshold);
    void filterAttacksByRelation(int relation, bool status);

    void setSize(const int &size ) { vert_size=size; }
    int size()  const { return vert_size; }

    void setShape(const QString &shape) { vert_shape=shape; }
    QString shape() const { return vert_shape; }

    void setColor(const QString &color) { vert_color=color; }
    QString color() const { return vert_color; }
    QString colorToPajek();

    void setX(const float &x) { vert_x=x; }
    float x() const { return vert_x; }

    void setY(const float &y) { vert_y=y; }
    float y() const { return vert_y; }

    QPointF pos () const { return QPointF ( x(), y() ); }

    //returns displacement vector
    QPointF & disp() { return vert_disp; }

    void set_dispX (float x) { vert_disp.rx() = x ; }
    void set_dispY (float y) { vert_disp.ry() = y ; }

    //slow
    void setOutLinkColor(const long int &v2, const QString &color) {
        outLinkColors[v2]=color;
    }
    //multi graph
    QString outLinkColor(const long int &v2) {
        if (outLinkColors.contains(v2))
            return outLinkColors.value(v2);
        else return "black";
    }

    void appendToPs(long  int vertex ) ;
    ilist Ps(void);

    //hold all outbound and inboud Attacks of this vertex.
    H_Attacks vert_outAttacks, vert_inAttacks;
signals:
    void setAttackVisibility (int, int, int, bool);

protected:

private:

    Graph *parentGraph;
    ilist myPs;
    long int vert_name,  vert_outAttacksCounter, vert_inAttacksCounter, vert_outDegree, vert_inDegree, vert_localDegree;
    float vert_Eccentricity;
    int vert_value, vert_size, vert_curRelation;
    H_StrToInt vert_cliques;
    bool vert_reciprocalLinked, vert_enabled, vert_hasCLC, vert_isolated;
    QString vert_color, vert_shape;
    QPointF vert_disp;
    H_IntToStr outLinkColors;
    //FIXME vertex coords::FIXED
    double vert_x, vert_y;

};

#endif
