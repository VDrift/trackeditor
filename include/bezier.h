/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _BEZIER_H

#include <iostream>
#include <fstream>
//#include "settings.h"
#include "utility.h"
#include "quat.h"

#include "globals.h"

#define COLLISION_DIVS 1
#define SURFDRAW_VIS 7

#define INTERSECT_FUNCTION utility.IntersectTriangleF

using namespace std;

class BEZIER
{
private:
	void DrawSurf(int div, float trans);
	void DrawControlPoints(float r, float g, float b, float a);
	VERTEX Bernstein(float u, VERTEX *p);
	VERTEX SurfCoord(float px, float py);

public:
	BEZIER();
	~BEZIER();
	void SetFromCorners(VERTEX fl, VERTEX fr, VERTEX bl, VERTEX br);
	void CalculateMiddleRows();
	void Visualize(bool wireframe, bool fill, VERTEX color);
	void Attach(BEZIER & other);

	int NumTris(int divs) {return divs*divs*2;}
	void GetTri(int divs, int num, VERTEX outtri[3]);
	
	bool Collide(VERTEX origin, VERTEX direction, VERTEX &outtri);
	
	void CopyFrom(BEZIER &other);
	
	bool ReadFrom(ifstream &openfile);
	bool WriteTo(ostream &openfile);

	VERTEX points[4][4];
	
	VERTEX center;
	float radius;
	void GenerateCenterAndRadius();
};

#define _BEZIER_H
#endif
