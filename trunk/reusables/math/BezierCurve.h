#ifndef realisim_math_bezier_curve_h
#define realisim_math_bezier_curve_h

#include "Point.h"
#include "Vect.h"
#include <vector>

namespace realisim
{
namespace math
{

/*Implémentation des courbes de bezier. Simplement, cette classe permet 
  de créer/modifier une courbe de bezier. Les courbes de bezier peuvent
  être de degré 1 (linéaire), de degré 2 (quadratique, de degré 3 
  (cubique) ou de degré supérieur à 3... L'implémentation actuelle
  supporte au maximum les courbe de degré 3, mais permet de raccorder
  plusieurs courbes cubiques ensemble permettant ainsi d'obtenir une
  courbe lisse et continu à plus de 4 points.
  
  La courbe est composée de points de contrôle (ControlPoint).
  Un point de contrôle représente une position spatiale ainsi que 
  l'information indiquant si ce point est directement sur la courbe ou
  plutôt la tangente à la courbe (ControlePoint::mIsHandleTo).

	méthode:
	La méthode addHandlesTo(int iIndex) permet d'ajouter une ou deux 
  poignées(handle) au point de contrôle indentifié par iIndex. 
  Un seul handle est ajouté lorsque iIndex représente le premier
  point de la courbe. Lorsque le point n'est pas le premier de la
  courbe, 2 handles sont ajoutés.
	
  membres:
    ControlPoint: Classe réprésante le point de contrôle.
      mIsHandleTo: L'index du point pour lequel ce point de
        contrôle est un "handle". L'index -1 indique que le point
        de contrôle n'est pas un "handle".
      mPos: La position 3d
    mControlPoints; La liste des points de contrôle de la courbe.
    mutable mRaster; La cache de la courbe rasterizée.
    mutable mIsRasterValid; Utilisé à l'interne pour déterminer si
      la rasterization est valide ou si elle doit être recalculée.
    mIsClosed; Détermine si la courve est fermée.
    mDummyPoint: Un point3d statique (0, 0, 0) qui permet à la
      méthode getPos(int iIndex) de retourner une référence. La statique 
      est utilisée quand l'index du point demandé n'est pas valide.

  Voir:
  http://en.wikipedia.org/wiki/Bézier_curve
  http://www.tsplines.com/resources/class_notes/Bezier_curves.pdf
    */
class BezierCurve
{
	public:
  	BezierCurve();    
    BezierCurve(const BezierCurve&);
    BezierCurve& operator=(const BezierCurve&);
    virtual ~BezierCurve();
  
  	enum {invalidPoint = -1};
    class ControlPoint 
    {
    	friend class BezierCurve;
      
    	public:
      	ControlPoint();
        ControlPoint(int, const Point3d&);
        ControlPoint(const ControlPoint&);
        ControlPoint& operator=(const ControlPoint&);
        ~ControlPoint();
        
        const Point3d& getPos() const {return mPos;}
        bool isHandle() const {return mIsHandleTo != -1;}
        int isHandleTo() const {return mIsHandleTo;}
                
      private:        
        void setAsHandleTo(int iIndex) {mIsHandleTo = iIndex;}
        void setPos(const Point3d& iP) {mPos = iP;}
        
        int mIsHandleTo;
        Point3d mPos;
    };
  
    int add(const Point3d&);
    int addHandlesTo(int);
    bool canCloseOn(int);
    void close();
    const std::vector<ControlPoint>& getControlPoints() const;
    const Point3d& getPos(int) const;
    const std::vector<Point3d>& getRaster() const;
    bool isClosed() const {return mIsClosed;}
    void move(int, const Vector3d&);    
    void remove(int);    
    
  protected:
    bool containsAtLeast2Points() const;
    void cubicRasterization(const std::vector<Point3d>&) const;
  	bool isRasterValid() const;
    void quadraticRasterization(const std::vector<Point3d>&) const;
    void rasterizeSegment(const std::vector<Point3d>& iV) const;
    void setPos(int, const Point3d&);
  
    std::vector<ControlPoint> mControlPoints;
    mutable std::vector<Point3d> mRaster;
    mutable bool mIsRasterValid;
    bool mIsClosed;
    static Point3d mDummyPoint;
};
  
} //math
} // enf of namespace realisim

#endif
