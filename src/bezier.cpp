/***************************************************************************
 *            bezier.cc
 *
 *  Sat Nov 12 10:20:06 2005
 *  Copyright  2005  Joe Venzon
 *  joe@venzon.net
 ****************************************************************************/

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
 
#include "bezier.h"

#include <algorithm>

BEZIER::BEZIER()
{
	int x,y;
	
	for (x = 0; x < 4; x++)
	{
		for (y = 0; y < 4; y++)
		{
			points[x][y].zero();
		}
	}
}

BEZIER::~BEZIER()
{
	
}

void BEZIER::SetFromCorners(VERTEX fl, VERTEX fr, VERTEX bl, VERTEX br)
{
	VERTEX temp;
	
	//assign corners
	points[0][0] = fl;
	points[0][3] = fr;
	points[3][3] = br;
	points[3][0] = bl;
	
	//calculate intermediate front and back points
	temp = fr - fl;
	points[0][1] = fl + temp.normalize().ScaleR(temp.len()/3.0);
	points[0][2] = fl + temp.normalize().ScaleR(2.0*temp.len()/3.0);
	temp = br - bl;
	points[3][1] = bl + temp.normalize().ScaleR(temp.len()/3.0);
	points[3][2] = bl + temp.normalize().ScaleR(2.0*temp.len()/3.0);
	
	//calculate intermediate left and right points	
	CalculateMiddleRows();
	
	GenerateCenterAndRadius();
}

void BEZIER::CalculateMiddleRows()
{
	for (int i = 0; i < 4; i++)
	{
		VERTEX temp = points[3][i] - points[0][i];
		points[1][i] = points[0][i] + temp.normalize().ScaleR(temp.len()/3.0);
		points[2][i] = points[0][i] + temp.normalize().ScaleR(2.0*temp.len()/3.0);
	}
	
	GenerateCenterAndRadius();
}

void BEZIER::Visualize(bool wireframe, bool fill, VERTEX color)
{
	if (!fill && !wireframe)
		return;
	
	//glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	
	if (fill)
	{
		glColor4f(1,1,1,1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		float trans = 0.25;
		glColor4f(1,1,1,trans);
		DrawSurf(SURFDRAW_VIS, trans);
		glEnable(GL_DEPTH_TEST);
	}
	
	if (wireframe)
	{
		glDisable(GL_DEPTH_TEST);
		glLineWidth(1.0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		//glColor4f(.5,.6,.5,0.1);
		DrawControlPoints(color.x, color.y, color.z, 0.5);
		
		/*glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glLineWidth(2.0);
		//glColor4f(color.x, color.y, color.z,1);
		DrawControlPoints(color.x, color.y, color.z, 1);*/
	}
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glPopAttrib();
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
}

void BEZIER::DrawSurf(int div, float trans)
{
	int x, y;
	
	float px, py, pxo, pyo, col;
	
	//VERTEX temp[4];
	VERTEX normal;
	
	if (div < 1)
		div = 1;
	
	glBegin(GL_TRIANGLES);
	
	for (x = 0; x < div; x++)
	{
		for (y = 0; y < div; y++)
		{
			//if (x < div - 1 && y < div - 1)
			{
				px = (float) x / (float) div;
				py = (float) y / (float) div;
				pxo = (float) (x+1) / (float) div;
				pyo = (float) (y+1) / (float) div;
				
				normal = (SurfCoord(pxo, pyo) - SurfCoord(px, py)).cross(SurfCoord(pxo, py) - SurfCoord(px, py));
				normal = normal.normalize();
				col = normal.y;
				col = col * col;
				glColor4f(col,col,col, trans);
				
				glVertex3fv(SurfCoord(pxo, py).v3());
				glVertex3fv(SurfCoord(px, py).v3());
				glVertex3fv(SurfCoord(px, pyo).v3());
				
				normal = (SurfCoord(px, pyo) - SurfCoord(px, py)).cross(SurfCoord(pxo, pyo) - SurfCoord(px, py));
				normal = normal.normalize();
				col = normal.y;
				col = col * col;
				glColor4f(col,col,col, trans);
				
				glVertex3fv(SurfCoord(px, pyo).v3());
				glVertex3fv(SurfCoord(pxo, pyo).v3());
				glVertex3fv(SurfCoord(pxo, py).v3());
			}
		}
	}

	glEnd();
}

void BEZIER::DrawControlPoints(float r, float g, float b, float a)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//glColor4f(1,0,0,1);
	
	/*int x, y;
	glPointSize(5.0);
	glBegin(GL_POINTS);
	
	for (x = 0; x < 4; x++)
	{
		for (y = 0; y < 4; y++)
		{
			glVertex3fv(points[x][y].v3());
		}
	}

	glEnd();*/
	
	/*float modamount = 0.0;
	glColor4f(r-modamount,g-modamount,b-modamount,0.1);
	
	glBegin(GL_QUADS);
	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			glVertex3fv(points[x][y].v3());
			glVertex3fv(points[x+1][y].v3());
			glVertex3fv(points[x+1][y+1].v3());
			glVertex3fv(points[x][y+1].v3());
		}
	}
	glEnd();
	
	glColor4f(r,g,b,a);
	
	glBegin(GL_QUADS);
	glVertex3fv(points[0][0].v3());
	glVertex3fv(points[3][0].v3());
	glVertex3fv(points[3][3].v3());
	glVertex3fv(points[0][3].v3());
	glEnd();*/
	
	glColor4f(r,g,b,a);
	glBegin(GL_QUADS);
	for (int x = 0; x < 3; x++)
	{
		glVertex3fv(points[0][x].v3());
		glVertex3fv(points[3][x].v3());
		glVertex3fv(points[3][x+1].v3());
		glVertex3fv(points[0][x+1].v3());
	}
	glEnd();
	
	/*glColor4f(r,g,b,a);
	glBegin(GL_QUADS);
	glVertex3fv(points[0][0].v3());
	glVertex3fv(points[3][0].v3());
	glVertex3fv(points[3][3].v3());
	glVertex3fv(points[0][3].v3());
	glEnd();*/
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void BEZIER::Attach(BEZIER & other)
{
	//move the other patch to the location of this patch
	/*for (x = 0; x < 4; x++)
		points[0][x] = other.points[3][x];*/
	
	//move the other patch to the location of this patch and force its
	// intermediate points into a nice grid layout
	//other.SetFromCorners(other.points[0][0], other.points[0][3], points[0][0], points[0][3]);
	
	VERTEX slope;
	
	for (int x = 0; x < 4; x++)
	{
		//slope points in the forward direction
		slope = (other.points[0][x] - points[3][x]).normalize();
		
		float otherlen = (other.points[0][x] - other.points[3][x]).len();
		float mylen = (points[0][x] - points[3][x]).len();
		
		//float meanlen = (otherlen + mylen)/2.0;
		float meanlen = std::min(otherlen, mylen);
		float leglen = meanlen / 3.0;
		
		other.points[2][x] = other.points[3][x] + slope.ScaleR(leglen);
		points[1][x] = points[0][x] + slope.ScaleR(-leglen);
	}
}

VERTEX BEZIER::Bernstein(float u, VERTEX *p)
{
	VERTEX	a, b, c, d, r;

	a = p[0].ScaleR(pow(u,3));
	b = p[1].ScaleR(3*pow(u,2)*(1-u));
	c = p[2].ScaleR(3*u*pow((1-u),2));
	d = p[3].ScaleR(pow((1-u),3));

	//r = pointAdd(pointAdd(a, b), pointAdd(c, d));
	r = a+b+c+d;

	return r;
}

VERTEX BEZIER::SurfCoord(float px, float py)
{
	VERTEX temp[4];
	VERTEX temp2[4];
	int i, j;

	//get splines along x axis
	for (j = 0; j < 4; j++)
	{
		for (i = 0; i < 4; i++)
			temp2[i] = points[j][i];
		temp[j] = Bernstein(px, temp2);
	}
	
	return Bernstein(py, temp);
}

void BEZIER::GetTri(int div, int num, VERTEX outtri[3])
{
	int firsttri = num % 2;
	
	int x = (num/2) % div;
	int y = (num/2) / div;

	if (firsttri == 0)
	{		
		float px = (float) x / (float) div;
		float py = (float) y / (float) div;
		float pxo = (float) (x+1) / (float) div;
		float pyo = (float) (y+1) / (float) div;
	
		/*glVertex3fv(SurfCoord(px, py).v3());
		glVertex3fv(SurfCoord(pxo, py).v3());
		glVertex3fv(SurfCoord(pxo, pyo).v3());*/
		
		outtri[0] = SurfCoord(px, py);
		outtri[1] = SurfCoord(pxo, py);
		outtri[2] = SurfCoord(px, pyo);
	}
	else
	{
		float px = (float) x / (float) div;
		float py = (float) y / (float) div;
		float pxo = (float) (x+1) / (float) div;
		float pyo = (float) (y+1) / (float) div;
		
		/*glVertex3fv(SurfCoord(px, py).v3());
		glVertex3fv(SurfCoord(pxo, pyo).v3());
		glVertex3fv(SurfCoord(px, pyo).v3());*/
		
		outtri[0] = SurfCoord(pxo, pyo);
		outtri[1] = SurfCoord(px, pyo);
		outtri[2] = SurfCoord(pxo, py);
	}
}

bool BEZIER::Collide(VERTEX origin, VERTEX direction, VERTEX &outtri)
{
	int i;
	
	VERTEX curtri[3];
	
	int retval = 0;
	
	float t, u, v;
	
	int x, y;
	
	for (i = 0; i < NumTris(COLLISION_DIVS) && !retval; i++)
	{	
		GetTri(COLLISION_DIVS, i, curtri);
		
		retval = INTERSECT_FUNCTION(origin.v3(), direction.v3(),
			curtri[0].v3(), curtri[1].v3(), curtri[2].v3(), 
			&t, &u, &v);
		
		if (retval)
		{
			if (i % 2 == 0)
			{
				u = 1.0 - u;
				v = 1.0 - v;
			}
			
			u = 1.0 - u;
			v = 1.0 - v;
			
			x = (i/2) % COLLISION_DIVS;
			y = (i/2) / COLLISION_DIVS;
			u += (float) x;
			v += (float) y;
			u = u / (float) COLLISION_DIVS;
			v = v / (float) COLLISION_DIVS;
			
			//u = 1.0 - u;
			//v = 1.0 - v;
			
			//cout << u << "," << v << endl;
			outtri = SurfCoord(u, v);
			return true;
		}
	}
	
	outtri = origin;
	return false;
}

void BEZIER::CopyFrom(BEZIER &other)
{
	int x, y;
	
	for (x = 0; x < 4; x++)
	{
		for (y = 0; y < 4; y++)
		{
			points[x][y] = other.points[x][y];
		}
	}
	
	center = other.center;
	radius = other.radius;
}

bool BEZIER::ReadFrom(ifstream &openfile)
{
	int x, y;
	
	if (!openfile)
		return false;
	
	for (x = 0; x < 4; x++)
	{
		for (y = 0; y < 4; y++)
		{
			openfile >> points[x][y].x;
			openfile >> points[x][y].y;
			openfile >> points[x][y].z;
		}
	}
	
	return true;
}

bool BEZIER::WriteTo(ostream &openfile)
{
	int x, y;
	
	for (x = 0; x < 4; x++)
	{
		for (y = 0; y < 4; y++)
		{
			openfile << points[x][y].x << " ";
			openfile << points[x][y].y << " ";
			openfile << points[x][y].z << endl;
		}
	}
	
	return true;
}

void BEZIER::GenerateCenterAndRadius()
{
	int i;
	VERTEX v[4];
	v[0] = points[0][0];
	v[1] = points[0][3];
	v[2] = points[3][0];
	v[3] = points[3][3];
	for (i=0;i<4;i++)
		center = center + v[i];
	center.Scale(0.25);
	radius = 0;
	for (i=0;i<4;i++)
	{
		float diff = (v[i] - center).len();
		if (diff > radius)
			radius = diff;
	}
}
