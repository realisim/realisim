using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Realisim.Core;
using Realisim.Core.Math;

namespace RealTraycer
{
  internal class Ray
  {
    private Point _org; // ray origine
    private Vector _dir; // ray direction

    private double _t; // distance
    private Point _hit; // coordinates of intersection
    private Vector _normal; // surface normal at point of intersection
    private Surface _surf; // intersected surface
    private Surface _parent; // surface hit by parent ray

    private double _fact; // coef lumiere
    private double _n; // indice de refraction du milieu courant
    private int _level; // depth of this branch of ray tree
    private uint _id; // numero du rayon

    public Color Hit()
    {
      return new Color();
    }
  }
}