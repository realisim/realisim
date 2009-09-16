using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Realisim.Core.Math;

namespace RealTraycer
{
  internal sealed class Data
  {
    private static volatile Data _instance;
    private static object syncRoot = new Object();

    private Dictionary<uint, Point> _points = new Dictionary<uint, Point>();
    private List<Triangle> _triangles = new List<Triangle>();


    private Data() 
    {
    }

    public static Data Instance
    {
      get
      {
        if (_instance == null)
        {
          lock (syncRoot)
          {
            if (_instance == null)
              _instance = new Data();
          }
        }

        return _instance;
      }
    }

    public Dictionary<uint, Point> Points
    {
      get { return _points; }
    }

    public List<Triangle> Triangles
    {
      get { return _triangles; }
    }

  }
}
