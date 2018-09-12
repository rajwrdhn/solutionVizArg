/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#ifndef GRAPH_H
#define GRAPH_H


using namespace std;

#include <QObject>
#include <QDateTime> 	// used in exporting centrality files
#include <QList>
#include <QHash>
#include <QTextStream>
#include <QThread>
#include <map>
#include "vertex.h"



class QPointF;


/**	This is the main class for a Graph, used in conjuction with Vertex.
    Graph class has the interface and the various graph algorithms,
    layout algorithms are also contained in graph.
*/

typedef QList<Vertex*> Vertices;

typedef QHash <QString, int> H_StrToInt;
typedef QHash <long int, long int> H_Int;

class Distance
{
public:
    int target;
    int distance;

    Distance(int t, int dist)
        : target(t), distance(dist)
    {

    }
};

struct Distance1 {
    int target;
    int distance;
};


// implementing a min-priority queue
class CompareDistances {
    public:
    bool operator()(Distance& t1, Distance& t2)
    {
       if (t1.distance == t2.distance)
            return t1.target > t2.target;
       return t1.distance > t2.distance;  //minimum priority
       // Returns true if t1 is closer than t2
       // else
    }
};

/**
 * @brief The Graph class
 */
class Graph:  public QObject {
    Q_OBJECT
public slots:

    /** Slots to signals from Parser */
    void createVertex(QString, int i, int size, QString ArgumentColor,
                        QPointF p, QString ArgumentShape, bool signalMW
                        , bool upload);//Main vertex creation call

    /** Slots to signals from GraphicsWidget and Parser*/
    void createAttack (QString argName_i, QString argName_j, int, int, float, QString, int, bool, bool, bool );	//GW and Parser.
    void createAttack (QString, QString, int, int, float, int, bool, bool );		//GW
    void createAttackUpload (QString argName_i, QString argName_j, int v1, int v2, float weight, int reciprocal, bool drawArrows, bool bezier);					//WebCrawler
    void ArgumentMovement(bool state, int type, int cW, int cH);		//Called by MW to start movement

    void slotSetAttackVisibility(int relation, int, int, bool);

    //auxiliary createVertex functions
    void createVertex(int i, QPointF p); 				//Called by GW
    void createVertex(int i, int canvasWidth, int canvasHeight); 	//Called by MW
    void createVertexUpload(bool, QString,int, int, int) ;

    /** Slots to signals from MainWindow */
    void setCanvasDimensions(int w, int h);
    void setGraphChanged(bool changed) { graphModified = changed; }
signals:
    /** Signals to MainWindow */
    void updateProgressDialog(int );
    void graphChanged();					//call to update MW widgets

    void statusMessage (QString message);			//updates statusbar message
    void describeDataset(QString);
    void signalArgumentSizesByOutDegree(bool);
    void signalArgumentSizesByInDegree(bool);

    /** Signals to GraphicsWidget */
    void drawArgument( QString, int ,int,  QString,
                   QPointF, QString, bool);	//call GW to draw a Argument

    void eraseArgument (long int);						//erase Argument from GW
    void drawAttack(QString, QString, int, int, float, bool, bool, QString, bool, bool);	//call GW to draw an Attack
    void eraseAttack(int, int);					//emited from removeAttack() to GW to clear the Attack item.
    void setAttackVisibility (int, int, int, bool);			// emitted from each Vertex
    void setVertexVisibility(long int, bool);		//notifies GW to disable a Argument
    void setArgumentSize(long int, int);
    void setArgumentShape(const long int, const QString);
    void setArgumentColor(long int, QString);
    void setArgumentLabel(long int, QString);
    void drawAttackReciprocal(QString,QString,int, int,bool);				//call GW to draw the Attack as symmetric one
    void changeAttackColor(long int, long int, QString);
    void addGuideCircle(int, int, int);				//call GW to draw a circular layout line somewhere.
    void addGuideHLine (int);					//call GW to draw a horizontal layout line somewhere.
    void signalRemoveGuides();
    void moveArgument(int, qreal, qreal);
    void moveArgument(int, qreal);

public:
    /* INIT AND CLEAR*/
    Graph();
    ~Graph();			//destroy object
    QString name();
    QStringList allArgumentList;
    /* VERTICES */
    int lastVertexNumber();
    int firstVertexNumber();

    int hasVertex(long int );
    int hasVertex(QString);
    void removeVertex (long int );

    void setInitVertexSize (const long int);
    void setVertexSize(const long int &v, const int &newsize );
    void setAllVerticesSize(const int &newsize);
    int vertexSize(const long int &v);

    void setInitVertexShape (const QString);
    void setVertexShape(const int v, const QString shape);
    void setAllVerticesShape(const QString shape);
    QString vertexShape(const int &v);

    void setInitVertexColor (const QString &color);
    void setVertexColor(const long &v, const QString &color);
    void setAllVerticesColor(const QString &color);
    QColor vertexColor(const long int &v);

    void setInitVertexNumberColor ( QString color);
    void setInitVertexNumberSize (int size);

    void setInitVertexLabelSize(int newSize);
    void setVertexLabelSize(int v, int newSize);

    void setInitVertexLabelColor(QString color);
    void setVertexLabel(int v, QString label);
    QString vertexLabel(const long int &v1);

    void updateVertCoords(int v, int x, int y);

    int vertices(const bool dropIsolates=false, const bool countAll=false) ;

    int outboundAttacks (int i) ;
    int inboundAttacks (int i) ;

    int outDegree(int);
    int inDegree(int);

    int verticesWithOutboundAttacks();
    int verticesWithInboundAttacks();
    int verticesWithReciprocalAttacks();

    QList<int> verticesIsolated();

    qreal euclideian_distance(const QPointF &a, const QPointF &b);
    qreal euclideian_distance(const QPointF &a);
    int sign(const qreal &D);

    qreal layoutForceDirected_F_rep(const qreal &dist,const qreal &optimalDistance) ;
    qreal layoutForceDirected_F_att(const qreal &dist,const qreal &optimalDistance) ;

    void layoutForceDirected_Eades_moveArguments(const qreal &c4);
    void layoutForceDirected_FR_moveArguments(const qreal &temperature) ;

    qreal computeOptimalDistance(const int &Vertices);
    void compute_angles( const QPointF &Delta,
                         const qreal &dist,
                         qreal &angle1,
                         qreal &angle2,
                         qreal &degrees1,
                         qreal &degrees2 );

    /* AttackS */
    int enabledAttacks();
    void Attacks();
    float hasArc (const long &v1, const long &v2);
    bool hasAttack (const int &v1, const long int &v2);
    void removeAttack (int v1, int v2);

    bool isWeighted();

    void setArcWeight (const long int &v1, const long int &v2, const float &w);
    void setInitAttackColor(const QString &);


    QString AttackColor (const long int &v1, const long int &v2);
    bool setAllAttacksColor(const QString &color);

    float density();

    bool symmetricAttack(int v1, int v2);
    bool isSymmetric();
    void symmetrize();

    void createAdjacencyMatrix(const bool dropIsolates=false,
                               const bool considerWeights=true,
                               const bool inverseWeights=false,
                               const bool symmetrize=false );
    bool invertAdjacencyMatrix(const QString &method);





    void centralityDegree(const bool weights, const bool dropIsolates=false);


    /* LAYOUTS */

    void layoutRandom( double maxWidth, double maxHeight );

    void layoutCircularRandom(double x0, double y0, double maxRadius);


    void layoutForceDirectedSpringEmbedder(bool &dynamicMovement);

    void layoutForceDirectedFruchtermanReingold(bool dynamicMovement);

    /**RANDOM*/
    void makeThingsLookRandom();
    int factorial (int);

    /**  index stores the real position of each vertex inside vert_graph.
     *  It starts at zero (0).
     *   We need to know the place of a vertex inside vert_graph after adding
     *   or removing many vertices
     */
    H_Int index;

    // Stores the number of vertices at distance n from a given vertex
    H_Int sizeOfNthOrderNeighborhood;

    /* maps have O(logN) lookup complexity */
    /* Consider using tr1::hashmap which has O(1) lookup, but this is not ISO C++ yet :(   */


    void clear();

protected:
    // Called from ArgumentMovement when a timerEvent occurs
    void timerEvent(QTimerEvent *event);


private:

    /**
     * List of pointers to the vertices.
     * A vertex stores all the info: links, colours, etc
    */
    Vertices vert_graph;



    /** private member functions */

    void addVertex (int v1, int val, int size, QString color,
            QPointF p, QString shape
            );

    void addAttack (int v1, int v2, float w, QString color, int reciprocal);

    /** methods used by createDistanceMatrix()  */
    void BFS(const int s, const bool computeCentralities,
             const bool dropIsolates);
    void dijkstra(const int s,const bool computeCentralities,
                  const bool inverseWeights, const bool dropIsolates);
    void minmax(
                float C, Vertex *v, float &max, float &min,
                int &maxArgument, int &minArgument
              ) ;

    QList<int>  vert_isolatedVerticesList;

    int vert_precision;

    /** General & initialisation variables */

    long int vert_totalVertices, initVertexSize;
    int initVertexLabelSize, initVertexNumberSize;

    int isolatedVertices;
    int outboundAttacksVert, inboundAttacksVert, reciprocalAttacksVert;
    int timerId,  layoutType, canvasWidth, canvasHeight;

    bool order, initNumbersInsideArguments;
    bool graphModified;


    QString VERSION, initAttackColor, initVertexColor,
        initVertexNumberColor, initVertexLabelColor, initVertexShape;
    bool dynamicMovement,symmetricadjacent;
    QDateTime actualDateTime;
};

#endif

