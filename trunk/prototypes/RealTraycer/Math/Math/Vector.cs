using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Realisim.Core.Math
{
  public class Vector
  {
    private double _x;
    private double _y;
    private double _z;
    private double _norm;

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

    public double Norm
    {
      get { return _norm; }
    }

    public Vector(Vector vec)
    {
      _x = vec._x;
      _y = vec._y;
      _z = vec._z;
    }

    public Vector(double x, double y, double z)
    {
      _x = x;
      _y = y;
      _z = z;
    }

    public Vector(Point pt1, Point pt2)
    {
      _x = pt2.X - pt1.X;
      _y = pt2.Y - pt1.Y;
      _z = pt2.Z - pt1.Z;
    }

    public Vector CrossProduct(Vector vec)
    {
      return new Vector(_y * vec._z - _z * vec._y, _z * vec._x - _x * vec._z, _x * vec._y - _y * vec._x);
    }

    public double DotProduct(Vector vec)
    {
      return _x * vec._x + _y * vec._y + _z * vec._z;
    }

    public void Normalise()
    {
      CalculateNorm();

      _x /= _norm;
      _y /= _norm;
      _z /= _norm;
    }

    public void DivideBy(double num)
    {
      _x /= num;
      _y /= num;
      _z /= num;
    }

    public void MultiplyBy(double num)
    {
      _x *= num;
      _y *= num;
      _z *= num;
    }

    public void AddVector(Vector vec)
    {
      _x += vec._x;
      _y += vec._y;
      _z += vec._z;
    }

    private void CalculateNorm()
    {
      _norm = System.Math.Sqrt(_x * _x + _y * _y + _z * _z);
    }

  }
}
