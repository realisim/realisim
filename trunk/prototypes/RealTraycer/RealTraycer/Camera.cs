using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Realisim.Core.Math;

namespace RealTraycer
{
  internal class Camera
  {
    private Point _origine; // --- def camera ---
    private Vector _eye;
    private Vector _up; // --- def camera ---
    private Point _rot;		// --- def camera ---
    private double _focale;	// --- def camera ---
    private double _fStop;	// --- def camera ---
    private double _scale;
  }
}
