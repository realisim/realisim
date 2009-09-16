using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Xml.Linq;
using Realisim.Core.Math;
using System.Globalization;

namespace RealTraycer
{
  internal class DataReader
  {
    public DataReader(string folderFileName)
    {
      CultureInfo cultureInfoEnUS = CultureInfo.CreateSpecificCulture("en-US");

      XDocument xmlDoc = XDocument.Load(folderFileName);

      var points = from point in xmlDoc.Descendants("point")
                   select new
                   {
                     x = point.Element("x").Value,
                     y = point.Element("y").Value,
                     z = point.Element("z").Value
                   };

      uint compteur = 0;
      double dx;
      double dy;
      double dz;
      foreach (var point in points)
      {
        double.TryParse(point.x, NumberStyles.Any, cultureInfoEnUS, out dx);
        double.TryParse(point.y, NumberStyles.Any, cultureInfoEnUS, out dy);
        double.TryParse(point.z, NumberStyles.Any, cultureInfoEnUS, out dz);

        Data.Instance.Points.Add(compteur, new Point(dx, dy, dz));
        compteur++;
      }

      var triangles = from triangle in xmlDoc.Descendants("polygon")
                      select new
                      {
                        pt1 = triangle.Element("pt1").Value,
                        pt2 = triangle.Element("pt2").Value,
                        pt3 = triangle.Element("pt3").Value,
                      };

      uint pt1;
      uint pt2;
      uint pt3;
      foreach (var triangle in triangles)
      {
        uint.TryParse(triangle.pt1, NumberStyles.Any, cultureInfoEnUS, out pt1);
        uint.TryParse(triangle.pt2, NumberStyles.Any, cultureInfoEnUS, out pt2);
        uint.TryParse(triangle.pt3, NumberStyles.Any, cultureInfoEnUS, out pt3);

        Data.Instance.Triangles.Add(
          new Triangle(
            Data.Instance.Points[pt1],
            Data.Instance.Points[pt2],
            Data.Instance.Points[pt3]));
      }

    }
  }
}
