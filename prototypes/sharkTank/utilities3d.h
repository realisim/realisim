#ifndef resonant_utilities_glUtilities_hh
#define resonant_utilities_glUtilities_hh

#include "coreLibrary, primitives.h"
#include "coreLibrary, types.h"

namespace resonant
{
namespace utilities
{

using namespace coreLibrary;

void drawAxis();
void drawBox(point3, point3);
void drawGrid(const plane&);
void drawLine(const line&);
void drawLineSegment(const lineSegment&);
void drawPoint(const point3&);
void drawPolygon(const polygon&);
void drawRectangle(point2, point2);

}
}
#endif