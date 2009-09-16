using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Realisim.Core.Math
{
  public class Point
  {
    private double _x;
    private double _y;
    private double _z;

    public double X
    {
      get { return _x; }
    }
    public double Y
    {
      get { return _y; }
    }
    public double Z
    {
      get { return _z; }
    }

    public Point(double x, double y, double z)
    {
      _x = x;
      _y = y;
      _z = z;
    }

  }
}
