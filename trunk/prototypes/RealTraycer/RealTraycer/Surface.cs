using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Realisim.Core;
using Realisim.Core.Math;

namespace RealTraycer
{
  internal class Surface
  {
    private string _name;
    private Matrix _transfo;
    private Matrix _inverse;
    private Color _color; // color properties
    
	  private double _kAmb; // coefficient eclairage ambiant
	  private double _kDiff; // coefficient of diffusion (Gouraud)

	  private double _kRefl; // coefficient of reflexion
	  private double _kRefr; // coefficient of refraction

	  private double _kSpec; // coefficient of reflect speculaire (Phong)
	  private double _shini; // fact. reflec. speculaire (Phong)

	  private double _n; // index of refraction inside
	  private double _blur; // pour le distribute ray-tracing (TP-#8)

	 
    public Surface()
    {
      // TODO: a faire
      //transfo.init();
      //col.set( 200, 100, 50 ) ;
      //kAmb  = 0.2 ;
      //kDiff = 0.8 ;
      //kRefr = kRefl = kSpec = 0.0 ;
    }

    public virtual double Intersect(Ray ray) { return 0; }
    public virtual Vector Normal(Point hit) { return null; }
    public virtual void PreProc() { ;}
  }

  internal class Triangle : Surface
  {
    private Point _a;
    private Point _b;
    private Point _c;

    private Vector _polyNormal;
    private Point _polyCenter;

    public Point PtA
    {
      get { return _a; }
    }
    public Point PtB
    {
      get { return _b; }
    }
    public Point PtC
    {
      get { return _c; }
    }
    public Vector PolygonNormal
    {
      get { return _polyNormal; }
    }
    public Point PolygonCenter
    {
      get { return _polyCenter; }
    }

    public Triangle(Point a, Point b, Point c)
    {
      _a = a;
      _b = b;
      _c = c;

      Vector tmp1 = new Vector(a, b);
      Vector tmp2 = new Vector(a, c);
      _polyNormal = tmp1.CrossProduct(tmp2);
      _polyNormal.Normalise();

      CalculatePolygonCenter();
    }

    public override double Intersect(Ray ray)
    {
      return base.Intersect(ray);
    }

    public override Vector Normal(Point hit)
    {
      return base.Normal(hit);
    }

    public override void PreProc()
    {
      base.PreProc();
    }

    private void CalculatePolygonCenter()
    {
      Point milieu;
      Vector v1;
      Vector v2;

      //Le CENTRE DE GRAVITE du polygone est situe au 2/3 d'une des medianes
      //Nous allons tracer la mediane a partir du pts[0]. il nous faut donc
      //le point milieu du segment pts[1],pts[2]
      v1 = new Vector(_b, _c);
      v1.DivideBy(2);

      v2 = new Vector(_b.X, _b.Y, _b.Z);
      v2.AddVector(v1);

      milieu = new Point(v2.X, v2.Y, v2.Z);

      //Trouvons la mediane
      v1 = new Vector(_a, milieu);
      v1.MultiplyBy(2 / 3);

      v2 = new Vector(_a.X, _a.Y, _a.Z);
      v2.AddVector(v1);

      _polyCenter = new Point(v2.X, v2.Y, v2.Z);
    }

    public bool IsFrontFacing(Triangle triangle)
    {
      // on prend en compte que les normales sont unitaires...
      double dot = _polyNormal.DotProduct(triangle._polyNormal);

      Vector vecProj = new Vector(_polyNormal);
      vecProj.MultiplyBy(dot);
      vecProj.Normalise();

      vecProj.AddVector(_polyNormal);

      if(vecProj.Norm >= -Definition.EPSILON && vecProj.Norm <= Definition.EPSILON)
        return true;
      else
        return false;
    }

  }
}