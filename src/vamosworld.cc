/***************************************************************************
 *            vamosworld.cc
 *
 *  Sat Mar 26 14:06:16 2005
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

#include "vamosworld.h"

extern void LoadingScreen(string loadtext);

extern void snap_screenshot();

void VAMOSWORLD::set_focused_car(int carnum)
{
	//focused_car = carnum;
}

VAMOSWORLD::VAMOSWORLD()
{
	display_hud = settings.GetHUDEnabled();
	//focused_car = 0;
	world = 0;
	
	error_log.open("logs/vamosworld.log");
	
	steerpos = 0;
	
	cammode = CMChaseRigid;
	
	joyinfo = false;
	
	propsteer = false;
	
	oldseg = 0;
	
	initdone = false;
	
	shift_time = 0;
}

void VAMOSWORLD::DeInit()
{
	if (initdone)
	{
		//clear_cars();
		//delete track_ptr;
		delete world;
		
		glDeleteTextures(1, &tachbase);
		glDeleteTextures(1, &tachredband);
		glDeleteTextures(1, &tachband);
		glDeleteTextures(1, &speedo);
		glDeleteTextures(1, &needle);
		glDeleteTextures(1, &fgbox);
		glDeleteTextures(1, &sphere_reflection);
		
		initdone = false;
	}
}

void VAMOSWORLD::add_car (Vamos_Body::Car* car)
{
	world->add_car(car);
}

void VAMOSWORLD::Init (Vamos_Track::Strip_Track* track)
{	
	//track->detailtex = utility.TexLoad("track2.png", GL_RGB, true);
	
	lastcamchange = 0.0f;
	
	world = new Vamos_World::World (track);
	
	tachbase = utility.TexLoad("hud/tachometer.png", GL_RGBA, false);
	tachredband = utility.TexLoad("hud/redband.png", GL_RGBA, false);
	tachband = utility.TexLoad("hud/tachband.png", GL_RGBA, false);
	speedo = utility.TexLoad("hud/speedo.png", GL_RGBA, false);
	needle = utility.TexLoad("hud/needle.png", GL_RGBA, false);
	fgbox = utility.TexLoad("hud/fuelgaugebox.png", GL_RGBA, false);
	sphere_reflection = utility.TexLoad("weather/reflect.png", GL_RGB, true);
	
	m_ballh = utility.TexLoad("hud/ball2.png", GL_RGBA, false);
	m_sliderh = utility.TexLoad("hud/slider2.png", GL_RGBA, false);
	
	m_ballv = utility.TexLoad("hud/accdec-marker.png", GL_RGBA, false);
	m_sliderv = utility.TexLoad("hud/accdec-slider.png", GL_RGBA, false);
	
	track_ptr = track;
	
	ifstream cf;
	int modenum = settings.GetCameraMode();
	switch (modenum)
	{
		//enum CameraMode { CMChaseRigid=0, CMChase=1, CMOrbit=2, CMHood=3, CMFree=4 };
		case 0:
			SetCameraMode(CMChaseRigid);
			break;
		case 1:
			SetCameraMode(CMChase);
			break;
		case 2:
			SetCameraMode(CMOrbit);
			break;
		case 3:
			SetCameraMode(CMHood);
			break;
		case 4:
			SetCameraMode(CMFree);
			break;
		case 5:
			SetCameraMode(CMInCar);
			break;
		case 6:
			SetCameraMode(CMExternal);
			break;
		case 7:
			SetCameraMode(CMExtFollow);
			break;
	}
	
	//Vamos_Geometry::Three_Vector cm = world->focused_car()->car->chassis().position();
	//cam.position.Set(-cm.m_vec[0], -cm.m_vec[2], cm.m_vec[1]);
	cam.position.Set(0,-terrain.GetHeight(0,0)-6,0);
	
	initdone = true;
}

extern bool verbose_output;
VAMOSWORLD::~VAMOSWORLD()
{
	if (verbose_output)
		cout << "vamosworld deinit" << endl;
	
	error_log.close();
	
	//if (world != 0)
		//delete world;
	
	DeInit();
}

extern GLfloat LightPosition[4];

void VAMOSWORLD::DrawShadows()
{
	glPushMatrix();
	
	QUATERNION goofyfoot;
	goofyfoot.Rotate(-3.141593/2.0, 1,0,0);
	double tempmat[16];
	goofyfoot.GetMat(tempmat);
	glMultMatrixd(tempmat);
	
	if (MP_DBGDEEP)
		cout << "car shadow start" << endl;
	draw_shadows();
	if (MP_DBGDEEP)
		cout << "car shadow done" << endl;
	
	glPopMatrix();
	
	trees.DrawShadows();
}

void VAMOSWORLD::Draw()
{
	//set_car_view (world->focused_car ()->car);
	//draw_track (true, world->focused_car ()->car->view_position ());
	//draw_cars (true);
	//draw_timing_info ();
	//draw_mirror_views ();
	
	glPushMatrix();
	
	QUATERNION goofyfoot;
	goofyfoot.Rotate(-3.141593/2.0, 1,0,0);
	double tempmat[16];
	goofyfoot.GetMat(tempmat);
	glMultMatrixd(tempmat);
	
	
	
	
	float lp[4];
	//lp[0] = -LightPosition[0];
	//lp[1] = LightPosition[2];
	//lp[2] = LightPosition[1];
	lp[0] = LightPosition[0];
	lp[1] = LightPosition[1];
	lp[2] = LightPosition[2];
	lp[3] = 0;
	VERTEX lpv;
	lpv.Set(lp);
	lpv = goofyfoot.ReturnConjugate().RotateVec(lpv);
	lp[0] = lpv.x;
	lp[1] = lpv.y;
	lp[2] = lpv.z;
	glLightfv( GL_LIGHT1, GL_POSITION,  lp);
	//lpv.DebugPrint();
	//glLightfv( GL_LIGHT1, GL_POSITION, lp );

	glPushMatrix();
	glEnable(GL_STENCIL_TEST);
	draw_track();
	glPopMatrix();
	
	/*lp[0] = LightPosition[0];
	lp[1] = LightPosition[1];
	lp[2] = LightPosition[2];
	lp[3] = 0;
	glLightfv( GL_LIGHT1, GL_POSITION, lp );*/
	
	/*if (MP_DBGDEEP)
		cout << "draw cars start" << endl;
	glPushMatrix();
	//glEnable(GL_STENCIL_TEST);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//glStencilFunc(GL_ALWAYS, 1, ~0);
	draw_cars(false, true);
	glPopMatrix();
	if (MP_DBGDEEP)
		cout << "draw cars done" << endl;*/
	
	glPopMatrix();
	
	lp[0] = LightPosition[0];
	lp[1] = LightPosition[1];
	lp[2] = LightPosition[2];
	lp[3] = 0;
	//glLightfv( GL_LIGHT1, GL_POSITION, lp );
}

void VAMOSWORLD::draw_track()
{
	glMatrixMode (GL_MODELVIEW);
	//cout << (void *) world->mp_track << endl;
	//glDisable(GL_TEXTURE_2D);
	world->mp_track->draw ();
}

void VAMOSWORLD::draw_cars(bool draw_interior, bool draw_focused_car)
{
	glMatrixMode (GL_MODELVIEW);
	//glPushAttrib(GL_ALL_ATTRIB_BITS);

	//glPolygonOffset(-2.0,-2.0);
	//glEnable(GL_POLYGON_OFFSET_FILL);
	
	/*glDisable(GL_DEPTH_TEST);
	glDepthMask(0);
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);
	
	//draw shadows, make tire sounds, create smoke
	for (std::vector <Vamos_World::Car_Information>::iterator it = world->m_cars.begin ();
	   it != world->m_cars.end ();
	   it++)
	{
	  assert (it->car != 0);
		glPushMatrix();
		VERTEX pos;
		Vamos_Geometry::Three_Vector cp = world->focused_car()->car->chassis().position();
		pos.Set(cp[0], cp[1], cp[2]);
		glTranslatef(pos.x, pos.y, 0);
		utility.SelectTU(0);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glDisable(GL_LIGHTING);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glBindTexture(GL_TEXTURE_2D, world->focused_car()->car->shadow_texture());
		
		float x1, x2, y1, y2;
		x1 = 3;
		x2 = -3;
		y1 = 3;
		y2 = -3;
		
		VERTEX wheelpos[4];//, wheelposxyz[4];
		
		int i = 0;
		
		double slide[4];
		for (i = 0; i < 4; i++)
		{
			Vamos_Geometry::Three_Vector tv = world->focused_car()->car->wheel(i)->contact_position();
			wheelpos[i].Set(tv[0],tv[1],tv[2]);
			//wheelposxyz[i].Set(tv[0],tv[2],-tv[1]);
			//wheelposxyz[i].DebugPrint();
			slide[i] = world->focused_car()->car->wheel(i)->slide();
		}
		//cout << endl;
		
		VERTEX coord[4];
		coord[0].Set(x1,y1,0);
		coord[1].Set(x1,y2,0);
		coord[2].Set(x2,y2,0);
		coord[3].Set(x2,y1,0);

		size_t segidx;
		QUATERNION rot;
		for (i = 0; i < 4; i++)
		{
			segidx = 0;
			double angle;
			Vamos_Geometry::Three_Vector aa = world->focused_car()->car->chassis().axis_angle(&angle);
			rot.SetAxisAngle(angle*(3.141593/180.0),aa[0],aa[1],aa[2]);
			coord[i] = rot.RotateVec(coord[i]);
			//coord[i].DebugPrint();
			//coord[i].z += 2.5;
		}
		
		//cout << coord[0].z << endl;
		
		Vamos_Geometry::Three_Vector cm = world->focused_car()->car->chassis().center_of_mass();
		//Vamos_Geometry::Three_Vector cm = world->focused_car()->car->chassis().position();
		VERTEX cmv;

		cmv.Set(cm[0],cm[1],cm[2]);
		for (i = 0; i < 4; i++)
		{
			//coord[i] = coord[i] - rot.RotateVec(cmv);
			coord[i].x = coord[i].x - rot.RotateVec(cmv).x;
			coord[i].y = coord[i].y - rot.RotateVec(cmv).y;
			
			//coord[i].DebugPrint();
			coord[i].z = world->mp_track->elevation(Vamos_Geometry::Three_Vector(coord[i].x+pos.x, coord[i].y+pos.y, coord[i].z), 0, world->focused_car()->segment_index);
			//coord[i].z += 0.02;
			//coord[i].z += 0.05;
			//cout << coord[i].z << endl;
		}

		VERTEX tirecoord[4], tc2[4];
		for (i = 0; i < 4; i++)
		{
			tirecoord[i] = rot.RotateVec(wheelpos[i]);
			tirecoord[i] = tirecoord[i] - rot.RotateVec(cmv);
			tirecoord[i] = pos + tirecoord[i];
			float temp = tirecoord[i].z;
			tirecoord[i].z = -tirecoord[i].y;
			//tirecoord.y = temp+particle.ParticleHeight();
			tirecoord[i].y = temp;
			tc2[i] = tirecoord[i];
		}
		
		VERTEX tctemp = tc2[2];
		tc2[2] = tc2[3];
		tc2[3] = tctemp;
		
		
		
		//check for particles drifting up into the car and kill them
		particle.ClipParticles(tc2);
		
		for (i = 0; i < 4; i++)
		{
			float prob = 0;
			//float probmultiple = 10;
			//float probmultiple = 40;
			float probmultiple = 100;
			if (slide[i] > 0)
			{
				prob = (l_timefactor / l_fps) * probmultiple;
				prob = prob * slide[i];
				if (prob < 0) //not sure this is necessary
					prob = -prob;
				VERTEX dir;
				dir.y = 1;
				particle.SetParams(1.0*slide[i], 2.0*slide[i], 1.0, 4.0,
					0.3, 1.0, dir, 0.5, 1.0);
				particle.ProbAddParticle(tirecoord[i], prob);
			}
			
			if (1)
			{
				float squeal = slide[i];
				if (squeal < 0)	//not sure this is necessary
					squeal = -squeal;
				float offset = 0.02;
				float gain = 0.5;
				squeal -= offset;
				squeal *= gain;
				if (squeal < 0)
					squeal = 0;
				if (squeal > 1)
					squeal = 1;
				if (l_timefactor != 0.0f)
					sound.SetGain(world->focused_car()->car->GetTireSoundSource(i), squeal);
				float pitch = 1.0-squeal;
				float pitchvariation = 0.2;
				pitch *= pitchvariation;
				pitch = pitch + (1.0-pitchvariation);
				sound.SetPitch(world->focused_car()->car->GetTireSoundSource(i), pitch);
				
				//sound.SetPos(tire_source[i], tirecoord[i]);
				
				VERTEX tvel = tirecoord[i] - oldtirepos[i];
				tvel.Scale(l_fps / l_timefactor);
				//sound.SetVel(tire_source[i], tvel);
				sound.SetPosVel(world->focused_car()->car->GetTireSoundSource(i), tirecoord[i], tvel);
				
				oldtirepos[i] = tirecoord[i];
			}
			//cout << slide[3] << endl;
			
			//cout << slide[i] << " ";
		}
		//cout << endl;
		
		
		//glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glDisable(GL_DEPTH_TEST);
		glColor4f(1,1,1,1);
		glEnable(GL_CULL_FACE);
		//glDisable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		//glColor4f(1,1,1,1);
		
		glBegin(GL_QUADS);
		
			glTexCoord2d(0,0);
			glVertex3fv(coord[0].v3());
			glTexCoord2d(0,1);
			glVertex3fv(coord[1].v3());
			glTexCoord2d(1,1);
			glVertex3fv(coord[2].v3());
			glTexCoord2d(1,0);
			glVertex3fv(coord[3].v3());
		
		glEnd();
		
		glDisable(GL_STENCIL_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glEnable(GL_LIGHTING);
		glPopMatrix();
		
	}*/
	
	//int i = 0;
	
	//glEnable(GL_STENCIL_TEST);
	for (std::vector <Vamos_World::Car_Information>::iterator it = world->m_cars.begin ();
	   it != world->m_cars.end ();
	   it++)
	{
	  	assert (it->car != 0);
		
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(1);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glDisable(GL_POLYGON_OFFSET_FILL);
		
		//draw cars
		/*if (utility.numTUs() > 1)
		{
			utility.SelectTU(1);
			glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); 
			glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); 
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, sphere_reflection);
			float sub[4];
			//sub[0] = sub[1] = sub[2] = 0.8f;
			sub[0] = sub[1] = sub[2] = 0.7f;
			//sub[0] = sub[1] = sub[2] = 0.0f;
			sub[3] = 0.0f;
			glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, sub);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
			glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB_ARB,GL_SRC_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB_ARB,GL_SRC_COLOR);
			//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, GL_CONSTANT_ARB);
			//glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND2_RGB_ARB,GL_SRC_COLOR);
			//glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_ARB, GL_INTERPOLATE);
			glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_ARB, GL_ADD);
			glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 1);
			
		}*/
		
		it->car->SetReflectionTexture(sphere_reflection);
		
		utility.SelectTU(0);
		//i++;
	  	//if (it->car != world->focused_car ()->car)
		//if (i == 2)
	    {
			glPushMatrix();
			
	      	it->car->draw (true);
	      	if (draw_interior)
	        {
	          	it->car->draw_interior ();
	        }
			glPopMatrix();
	    }
		
		utility.SelectTU(0);
		glPopAttrib();
	}
	
	/*if (draw_focused_car)
	{
		glPushMatrix();
	  world->focused_car ()->car->draw ();
	  if (draw_interior)
	    {
	      world->focused_car ()->car->draw_interior ();
	    }
		glPopMatrix();
	}*/
	
	//glPopAttrib();
}

void VAMOSWORLD::draw_shadows()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glMatrixMode (GL_MODELVIEW);

	//glPolygonOffset(-2.0,-2.0);
	//glEnable(GL_POLYGON_OFFSET_FILL);
	
	glDisable(GL_DEPTH_TEST);
	glDepthMask(0);
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);
	
	int count = 0;

	//draw shadows, make tire sounds, create smoke
	for (std::vector <Vamos_World::Car_Information>::iterator it = world->m_cars.begin ();
	   it != world->m_cars.end ();
	   it++)
	{
		if (MP_DBGDEEP)
			cout << "wheel pos start" << endl;
		
	  	assert (it->car != 0);
		
		if (it->car->chassis().IsValid())
		{		
			glPushMatrix();
			VERTEX pos;
			Vamos_Geometry::Three_Vector cp = it->car->chassis().position();
			pos.Set(cp[0], cp[1], cp[2]);
			//pos.DebugPrint();
			glTranslatef(pos.x, pos.y, 0);
			utility.SelectTU(0);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glDisable(GL_LIGHTING);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			glBindTexture(GL_TEXTURE_2D, it->car->shadow_texture());
			
			float x1, x2, y1, y2;
			x1 = 3;
			x2 = -3;
			y1 = 3;
			y2 = -3;
			
			VERTEX wheelpos[4];//, wheelposxyz[4];
			
			int i = 0;
			
			double slide[4];
			//pos.DebugPrint();
			for (i = 0; i < 4; i++)
			{
				Vamos_Geometry::Three_Vector tv = it->car->wheel(i)->contact_position();
				wheelpos[i].Set(tv[0],tv[1],tv[2]);
				//wheelposxyz[i].Set(tv[0],tv[2],-tv[1]);
				//wheelpos[i].DebugPrint();
				slide[i] = it->car->wheel(i)->slide();
				//slide[i] = slide[i] / 10.0;
				//cout << slide[i] << endl;
			}
			//cout << endl;
			/*VERTEX wptemp = wheelposxyz[2];
			wheelposxyz[2] = wheelposxyz[3];
			wheelposxyz[3] = wptemp;*/
			/*
			x1 = 0;
			x2 = 0;
			y1 = 0;
			y2 = 0;
			
			for (i = 0; i < 4; i++)
			{
				if (wheelpos[i].x < x2)
					x2 = wheelpos[i].x;
				if (wheelpos[i].x > x1)
					x1 = wheelpos[i].x;
				if (wheelpos[i].y > y1)
					y1 = wheelpos[i].y;
				if (wheelpos[i].y < y2)
					y2 = wheelpos[i].y;
			}
			
	
			x1 += 1.3;
			x2 -= 0.25;
			y1 += 0.5;
			//y2 -= 0.285;
			y2 -= 0.33;
			*/
			VERTEX coord[4];
			coord[0].Set(x1,y1,0);
			coord[1].Set(x1,y2,0);
			coord[2].Set(x2,y2,0);
			coord[3].Set(x2,y1,0);
	
			if (MP_DBGDEEP)
				cout << "wheel pos done" << endl;
	
			size_t segidx;
			QUATERNION rot;
			for (i = 0; i < 4; i++)
			{
				segidx = 0;
				double angle;
				Vamos_Geometry::Three_Vector aa = it->car->chassis().axis_angle(&angle);
				rot.SetAxisAngle(angle*(3.141593/180.0),aa[0],aa[1],aa[2]);
				coord[i] = rot.RotateVec(coord[i]);
				//coord[i].DebugPrint();
				//coord[i].z += 2.5;
			}
			
			//cout << coord[0].z << endl;
			
			Vamos_Geometry::Three_Vector cm = it->car->chassis().center_of_mass();
			//Vamos_Geometry::Three_Vector cm = world->focused_car()->car->chassis().position();
			VERTEX cmv;
	
			if (MP_DBGDEEP)
				cout << "coord adjust start" << endl;
			
			cmv.Set(cm[0],cm[1],cm[2]);
			//cmv.DebugPrint();
			for (i = 0; i < 4; i++)
			{
				//coord[i] = coord[i] - rot.RotateVec(cmv);
				coord[i].x = coord[i].x - rot.RotateVec(cmv).x;
				coord[i].y = coord[i].y - rot.RotateVec(cmv).y;
				
				coord[i].z = world->mp_track->elevation(Vamos_Geometry::Three_Vector(coord[i].x+pos.x, coord[i].y+pos.y, coord[i].z), 0, it->segment_index);
				//coord[i].DebugPrint();
				//coord[i].z += 0.02;
				//coord[i].z += 0.05;
				//cout << coord[i].z << endl;
			}
	
			VERTEX tirecoord[4], tc2[4];
			for (i = 0; i < 4; i++)
			{
				tirecoord[i] = rot.RotateVec(wheelpos[i]);
				tirecoord[i] = tirecoord[i] - rot.RotateVec(cmv);
				tirecoord[i] = pos + tirecoord[i];
				float temp = tirecoord[i].z;
				tirecoord[i].z = -tirecoord[i].y;
				//tirecoord.y = temp+particle.ParticleHeight();
				tirecoord[i].y = temp;
				tc2[i] = tirecoord[i];
				
				//tirecoord[i].DebugPrint();
			}
			
			VERTEX tctemp = tc2[2];
			tc2[2] = tc2[3];
			tc2[3] = tctemp;
			
			/*for (i = 0; i < 4; i++)
				tc2[i].DebugPrint();
			cout << endl;*/
			
			//check for particles drifting up into the car and kill them
			//particle.ClipParticles(tc2);
			
			if (MP_DBGDEEP)
				cout << "coord adjust stop" << endl;
			
			if (count == 0 || multiplay.TickCar(count))
			{
			
				for (i = 0; i < 4; i++)
				{
					float prob = 0;
					//float probmultiple = 10;
					//float probmultiple = 40;
					float probmultiple = 100;
					float maxprob = 0.1;
					if (slide[i] > 0)
					{
						prob = (l_timefactor / l_fps) * probmultiple;
						prob = prob * slide[i];
						if (prob < 0) //not sure this is necessary
							prob = -prob;
						VERTEX dir;
						dir.y = 1;
						particle.SetParams(1.0*slide[i], 2.0*slide[i], 5.0, 14.0,
							0.3, 1.0, dir, 0.5, 1.0);
						if (prob > maxprob)
							prob = maxprob;
						particle.ProbAddParticle(tirecoord[i], prob);
					}
		
					if (1)
					{
						float squeal = slide[i];
						if (squeal < 0)	//not sure this is necessary
							squeal = -squeal;
						float offset = 0.04;
						float gain = 0.5;
						squeal -= offset;
						squeal *= gain;
						if (squeal < 0)
							squeal = 0;
						if (squeal > 1)
							squeal = 1;
						
						bool snddebug = false;
						float maxgain = 0.3;
						gain = squeal;
						if (gain > maxgain)
							gain = maxgain;
						if (l_timefactor != 0.0f)
							if (!snddebug) sound.SetGain(it->car->GetTireSoundSource(i), gain);
						float pitch = 1.0-squeal;
						float pitchvariation = 0.4;
						pitch *= pitchvariation;
						pitch = pitch + (1.0-pitchvariation);
						if (!snddebug) sound.SetPitch(it->car->GetTireSoundSource(i), pitch);
						
						//sound.SetPos(tire_source[i], tirecoord[i]);
						
						/*VERTEX tvel = tirecoord[i] - oldtirepos[i];
						tvel.Scale(l_fps / l_timefactor);
						//sound.SetVel(tire_source[i], tvel);*/
						VERTEX tvel;
						if (!tirecoord[i].nan() && !tvel.nan())
							if (!snddebug) sound.SetPosVel(it->car->GetTireSoundSource(i), tirecoord[i], tvel);
						
						oldtirepos[i] = tirecoord[i];
					}
					//cout << slide[3] << endl;
					//cout << slide[i] << " ";
				}
			}
			//cout << endl;
			
			/*VERTEX offsetcoord[4];
			for (i = 0; i < 4; i++)
			{
				offsetcoord[i] = coord[i];
				//offsetcoord[i].z += 0.2;
			}
		
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_STENCIL_TEST);
			glStencilFunc(GL_ALWAYS, 1, 1);
			glStencilOp(GL_KEEP, GL_ZERO, GL_REPLACE);
			//glColorMask(0, 0, 0, 0);
			glDepthMask(1);
			//glDisable(GL_BLEND);
			glBegin(GL_QUADS);
				glTexCoord2d(0,0);
				glVertex3fv(offsetcoord[0].v3());
				glTexCoord2d(0,1);
				glVertex3fv(offsetcoord[1].v3());
				glTexCoord2d(1,1);
				glVertex3fv(offsetcoord[2].v3());
				glTexCoord2d(1,0);
				glVertex3fv(offsetcoord[3].v3());
			glEnd();
			
			glStencilFunc(GL_EQUAL, 1, 1);
			glEnable(GL_BLEND);
			glStencilMask(GL_FALSE);
			glDisable(GL_DEPTH_TEST);
			//glDepthMask(1);
			//glColorMask(1,1,1,1);*/
			//glEnable(GL_DEPTH_TEST);
			//glEnable(GL_STENCIL_TEST);
			glDisable(GL_DEPTH_TEST);
			glColor4f(1,1,1,1);
			glEnable(GL_CULL_FACE);
			//glDisable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			//glColor4f(1,1,1,1);
			
			glBegin(GL_QUADS);
			
				glTexCoord2d(0,0);
				glVertex3fv(coord[0].v3());
				glTexCoord2d(0,1);
				glVertex3fv(coord[1].v3());
				glTexCoord2d(1,1);
				glVertex3fv(coord[2].v3());
				glTexCoord2d(1,0);
				glVertex3fv(coord[3].v3());
			
			glEnd();
			
			//glDisable(GL_STENCIL_TEST);
			glDisable(GL_CULL_FACE);
			glDisable(GL_BLEND);
			glEnable(GL_LIGHTING);
			glPopMatrix();
		}
	}
	
	glEnable(GL_DEPTH_TEST);
	glDepthMask(1);
	glDisable(GL_CULL_FACE);
	//glDisable(GL_POLYGON_OFFSET_FILL);
	
	utility.SelectTU(0);
		
	utility.SelectTU(1);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_2D);
	utility.SelectTU(0);
	glDepthMask(1);
	
	glPopAttrib();
}

void VAMOSWORLD::Update(float timefactor, float fps, SDL_Joystick ** js)
{	
	l_timefactor = timefactor;
	l_fps = fps;
	
	if (l_timefactor <= 0 || fps < 5)
	{
		sound.MuteAll();
		return;
	}
	
	//check for a lap
	//float dist = world->mp_track->distance (world->focused_car()->car->chassis ().position (), world->focused_car()->segment_index);
	
	for (std::vector <Vamos_World::Car_Information>::iterator it = world->m_cars.begin ();
	it != world->m_cars.end ();
	it++)
	{
		const double distance = 
		world->mp_track->distance (it->car->chassis ().position (), it->segment_index);
		const int sector = world->mp_track->sector (distance);
		//it->timing.update (mp_timer->get_time_step (), distance, sector);
		it->timing.update (timefactor/fps, distance, sector);
		
		//cout << it->segment_index << "," << distance << endl;
		//cout << it->car->chassis ().position ().magnitude() << endl;
	}
	
	int curseg = GetCar(CONT_PLAYERLOCAL)->segment_index;
	int numsegs = world->mp_track->get_segments().size();
	//cout << dist << endl;
	if (oldseg == numsegs-1 && curseg == 0 && replay.Playing() == -1)
		timer.Lap();
	oldseg = curseg;
	
	bool camswitch = false;
	
	//cout << "not paused" << endl;
	
	ProcessControls(js, timefactor, fps);
	
	const int repeat = 1;
	//const double dt = mp_timer->get_substep ();
	double dt = (timefactor/fps)/(float)repeat;
	for (int loop = 0; loop < repeat; loop++)
	{
	  PhysUpdate(dt);
	}
	dt = (timefactor/fps);
	
	if (GetCar(CONT_PLAYERLOCAL) != 0)
	{
		
		//set camera position
		Vamos_Geometry::Three_Vector cm = GetCar(CONT_PLAYERLOCAL)->car->chassis().position();
		//Vamos_Geometry::Three_Vector cm = world->focused_car()->car->chassis().center_of_mass();
		if (cammode != CMFree && cammode != CMChase && cammode != CMExternal && cammode != CMExtFollow)
		{
			cam.position.Set(-cm[0], -cm[2], cm[1]);
			//cam.position.Set(-cm.m_vec[0], -cm.m_vec[2], 0.0);
			//cam.position.DebugPrint();
		}
		
		double angle;
		Vamos_Geometry::Three_Vector axis = GetCar(CONT_PLAYERLOCAL)->car->chassis().axis_angle (&angle);
		Vamos_Geometry::Three_Vector vvel = GetCar(CONT_PLAYERLOCAL)->car->chassis().cm_velocity();
		VERTEX vel;
		vel.Set(vvel[0],vvel[1],vvel[2]);
		
		if (cammode == CMChase || cammode == CMChaseRigid || cammode == CMHood || cammode == CMInCar)
		{
			QUATERNION carorientation;
			carorientation.SetAxisAngle(angle*(3.141593/180), -axis[0], -axis[2], axis[1]);
			carorientation.Rotate(3.141593/2.0,0,1,0);			
			if (cammode == CMChase)
			{
				VERTEX idealpos;
				idealpos.Set(-cm[0], -cm[2], cm[1]);
				//idealpos.Set(-cm.m_vec[0], -cm.m_vec[2], 0.0);
				VERTEX temp;
				temp.Set(0,-2,0);
				idealpos = idealpos + carorientation.RelativeMove(temp);
				temp.Set(0,0,-7.5);
				idealpos = idealpos + carorientation.RelativeMove(temp);
				
				//float veldamp = 0.1f;
				//VERTEX dvel = vel.ScaleR(veldamp);
				//cam.Move(dvel.x, -dvel.z, -dvel.y);
				float dirblend = 1.0;
				float posblend = 10.0*(timefactor/fps);
				if (dirblend < 0)
					dirblend = 0;
				else if (dirblend > 1)
					dirblend = 1;
				if (posblend < 0)
					posblend = 0;
				else if (posblend > 1)
					posblend = 1;
				//cam.dir = carorientation.QuatSlerp(carorientation, dirblend);
				cam.position = cam.position.interpolatewith(idealpos, posblend);
				cam.dir.LookAt(-cam.position.x, -cam.position.y, -cam.position.z, cm[0], cm[2]+2, -cm[1], 0,1,0);
				cam.dir = cam.dir.ReturnConjugate();
				
				//cout << "CMChase" << endl;
				//cam.dir.DebugPrint();
			}
			else if (cammode == CMHood)
			{
				cam.dir = carorientation;
				
				Vamos_Geometry::Three_Vector vp = GetCar(CONT_PLAYERLOCAL)->car->view_position();
				vp = vp - GetCar(CONT_PLAYERLOCAL)->car->chassis().center_of_mass();
				
				cam.dir = carorientation;
				//cam.MoveRelative(0,-0.9,0);
				//cam.MoveRelative(0,0,-0.25);
				cam.MoveRelative(0.0,
								-vp[2],
								vp[1]+1.0);
				//cam.MoveRelative(0.0,-world->focused_car()->car->view_position()[1],
				//				-world->focused_car()->car->view_position()[2]+1.0);
			}
			else if (cammode == CMInCar)
			{
				Vamos_Geometry::Three_Vector vp = GetCar(CONT_PLAYERLOCAL)->car->view_position();
				vp = vp - GetCar(CONT_PLAYERLOCAL)->car->chassis().center_of_mass();
				//cout << world->focused_car()->car->chassis().center_of_mass()[0] << endl;
				cam.dir = carorientation;
				
				VERTEX idealpos, viewpos;
				viewpos.Set(-vp[0], -vp[2], vp[1]);
				idealpos.Set(-cm[0], -cm[2], cm[1]);
				//idealpos = idealpos + viewpos;
				idealpos = idealpos + carorientation.RelativeMove(viewpos);
				//cam.MoveRelative(viewpos.x, viewpos.y, viewpos.z);
				/*idealpos.x = -vp[1];
				idealpos.y = -vp[2];
				idealpos.z = vp[0];*/
				
				if (camneedupdate)
				{
					cam.position = idealpos;
					cam_lastpos = cam.position;
					cam_lastpos2 = cam.position;
					camneedupdate = false;
					cam_lastvel.zero();
					cam_lastaccel.zero();
					cam_jerk.zero();
				}
				
				VERTEX accel;
				//vp = GetCar(CONT_PLAYERLOCAL)->car->chassis().cm_velocity();// - GetCar(CONT_PLAYERLOCAL)->car->chassis().cm_last_velocity();
				VERTEX vel;
				//vel.Set(-vp[0], -vp[2], vp[1]);
				vel = cam.position - cam_lastpos2;
				//vel.DebugPrint();
				vel.Scale(1.0/dt);
				//vel.DebugPrint();
				accel = cam_lastvel - vel;
				cam_lastvel = vel;
				assert (timefactor != 0);
				accel.Scale(1.0/dt);
				accel = cam_lastaccel + (accel-cam_lastaccel).ScaleR(0.002);
				//cam.position.DebugPrint();
				//cam_lastpos2.DebugPrint();
				cam_lastpos2 = cam.position;
				//cout << "  ->" << cam_lastpos.len() << "," << vel.len() << "," << accel.len() << endl;
				
				//accel.DebugPrint();
				
				VERTEX jerk;
				//jerk = accel - cam_lastaccel;
				jerk = accel - cam_lastaccel;
				jerk.Scale(1.0/dt);
				cam_jerk = cam_jerk + (jerk-cam_jerk).ScaleR(0.002);
				//cam_jerk = cam_jerk + (jerk-cam_jerk).ScaleR(0);
				cam_lastaccel = accel;
				//cout << cam_jerk.len() << endl;
				
				jerk = accel;
				jerk = cam_jerk;
				
				//cout << jerk.len() << endl;
				
				//acceleration displacement constants
				float jerkscale = 0.015;
				float jerkmax = 0.1;
				
				jerk.Scale(jerkscale);
				if (jerk.x >= 0)
				{
					if (jerk.x > jerkmax)
						jerk.x = jerkmax;
				}
				else
				{
					if (jerk.x < -jerkmax)
						jerk.x = -jerkmax;
				}
				if (jerk.y >= 0)
				{
					if (jerk.y > jerkmax)
						jerk.y = jerkmax;
				}
				else
				{
					if (jerk.y < -jerkmax)
						jerk.y = -jerkmax;
				}
				if (jerk.z >= 0)
				{
					if (jerk.z > jerkmax)
						jerk.z = jerkmax;
				}
				else
				{
					if (jerk.z < -jerkmax)
						jerk.z = -jerkmax;
				}
				
				idealpos = idealpos + jerk;
				
				/*float posblend = 10.0*(timefactor/fps);
				if (posblend < 0)
					posblend = 0;
				else if (posblend > 1)
					posblend = 1;

				cam.position = cam.position.interpolatewith(idealpos, posblend);*/
				
				cam.position.Set(idealpos.x, idealpos.y, idealpos.z);
				
				/*cam.MoveRelative(-vp[1],
								-vp[2],
								vp[0]);*/
				//cam.position = idealpos;
				
			}
			else //CMChaseRigid
			{
				cam.dir = carorientation;
				cam.MoveRelative(0,-2.0,0);
				cam.MoveRelative(0,0,-7.75);
			}
		}
		else if (cammode == CMOrbit)
		{
			float mindist = 7;
			float maxdist = 20;
			
			cam.MoveRelative(0,-1.0,0);
			cam.MoveRelative(0,0,-(mindist*(mouse.GetZoom())+maxdist*(1.0f-mouse.GetZoom())));
			
			cam.dir = mouse.GetDir();
		}
		else if (cammode == CMFree)
		{
			cam.dir = mouse.GetDir();
		}
		else if (cammode == CMExtFollow)
		{	
			float cheight = 4.0f;
			float chasedist = 10.0f;
			
			VERTEX carpos;
			carpos.Set(cm[0], cm[2], -cm[1]);
			
			size_t segnum = 0;
			
			//segnum = world->mp_track->getsegment(cm);
			//world->mp_track->distance(cm, segnum);
			//world->mp_track->distance(cm, world->focused_car()->segment_index);
			segnum = GetCar(CONT_PLAYERLOCAL)->segment_index;
			//cout << segnum << ":" << world->mp_track->get_segments()[segnum]->length() << "," << world->mp_track->get_segments()[segnum]->radius() << "," << world->mp_track->get_segments()[segnum]->arc() << endl;
			
			Vamos_Geometry::Three_Vector sc, ec, tp;
			sc = world->mp_track->get_segments()[segnum]->start_coords();
			ec = world->mp_track->get_segments()[segnum]->end_coords();
			world->mp_track->get_segments()[segnum]->coordinates(cm, tp);
			double along = tp[0];
			double length = world->mp_track->get_segments()[segnum]->length();
			VERTEX startcoords, endcoords;
			startcoords.Set(sc[0], sc[2], -sc[1]);
			endcoords.Set(ec[0], ec[2], -ec[1]);
			
			VERTEX newpos;
			//newpos = startcoords + endcoords;
			//newpos.Scale(0.5);
			newpos = startcoords.interpolatewith(endcoords, along / length);
			
			VERTEX off1;
			off1 = endcoords - startcoords;
			off1.Scale(1.0 / off1.len());
			
			size_t segnext = segnum + 1;
			if (segnext == world->mp_track->get_segments().size())
				segnext = 0;
			sc = world->mp_track->get_segments()[segnext]->start_coords();
			ec = world->mp_track->get_segments()[segnext]->end_coords();
			startcoords.Set(sc[0], sc[2], -sc[1]);
			endcoords.Set(ec[0], ec[2], -ec[1]);
			
			VERTEX off2;
			off2 = endcoords - startcoords;
			off2.Scale(1.0 / off2.len());
			
			VERTEX offset;
			offset = off1.interpolatewith(off2, along / length);
			
			//newpos = newpos - off1.ScaleR(chasedist);
			newpos = newpos - offset.ScaleR(chasedist);
			newpos.y = terrain.GetHeight(newpos.x, newpos.z) + cheight;
			
			//oldcampos = oldcampos.interpolatewith(newpos, 0.001);
			oldcampos = oldcampos.interpolatewith(newpos, 0.005);
			
			if (camneedupdate)
			{
				oldcampos = newpos;
				camneedupdate = false;
			}
			
			VERTEX zero;
			cam.SetPosition(zero - oldcampos);
			
			//camera alignment
			QUATERNION lookat;
			//lookat.LookAt(newpos.x, newpos.y, newpos.z, carpos.x, newpos.y, carpos.z, 0, 1, 0);
			lookat.LookAt(oldcampos.x, oldcampos.y, oldcampos.z, carpos.x, carpos.y, carpos.z, 0, 1, 0);
			cam.dir = lookat.ReturnConjugate();
		}
		else if (cammode == CMExternal)
		{
			lastcamchange += timefactor/fps;
			
			float cheight = 4.0f;
			
			VERTEX carpos;
			carpos.Set(cm[0], cm[2], -cm[1]);
			
			size_t segnum = 0;
			
			//segnum = world->mp_track->getsegment(cm);
			//world->mp_track->distance(cm, segnum);
			//world->mp_track->distance(cm, world->focused_car()->segment_index);
			segnum = GetCar(CONT_PLAYERLOCAL)->segment_index;
			
			Vamos_Geometry::Three_Vector sc, ec;
			sc = world->mp_track->get_segments()[segnum]->start_coords();
			ec = world->mp_track->get_segments()[segnum]->end_coords();
			VERTEX startcoords, endcoords;
			//cout << segnum << ":" << world->mp_track->get_segments()[segnum]->length() << "," << world->mp_track->get_segments()[segnum]->radius() << "," << world->mp_track->get_segments()[segnum]->arc() << endl;
			//startcoords.DebugPrint();
			//endcoords.DebugPrint();
			startcoords.Set(sc[0], sc[2], -sc[1]);
			endcoords.Set(ec[0], ec[2], -ec[1]);
			
			VERTEX newpos;
			newpos = startcoords + endcoords;
			newpos.Scale(0.5);
			newpos.x += 10;
			newpos.z += 10;
			//newpos = startcoords;
			
			newpos.y = terrain.GetHeight(newpos.x, newpos.z) + cheight;
			
			if (lastcamchange > 1.5 || camneedupdate)
			{
				camneedupdate = false;
				
				lastcamchange = 0.0f;
				VERTEX zero;
				cam.SetPosition(zero - newpos);
				oldcampos = newpos;
				camswitch = true;
			}
			
			//camera alignment
			QUATERNION lookat;
			//lookat.LookAt(oldcampos.x, oldcampos.y, oldcampos.z, carpos.x, oldcampos.y, carpos.z, 0, 1, 0);
			lookat.LookAt(oldcampos.x, oldcampos.y, oldcampos.z, carpos.x, carpos.y, carpos.z, 0, 1, 0);
			cam.dir = lookat.ReturnConjugate();
		}
		
		if (cammode != CMFree && cammode != CMOrbit)
		{
			mouse.InitDir(cam.dir);
		}
		
		float theight = terrain.GetHeight(-cam.position.x, -cam.position.z);
		if (-cam.position.y < theight + 0.6)
			cam.position.y = -theight - 0.6;
	}
	
	/*vel.z = 0;
	vel = vel.normalize();
	vel.Scale(3.0);
	cam.Move(0,-2.0,0);
	cam.Move(vel.x, 0, -vel.y);*/
	
	int count = 0;
	
	for (std::vector <Vamos_World::Car_Information>::iterator it = world->m_cars.begin ();
	it != world->m_cars.end ();
	it++)
	{
		Vamos_Geometry::Three_Vector cm = it->car->chassis().position();
		
		//update sounds
		int sid = it->car->GetSoundSource();
		//int sid = world->focused_car ()->car->engine_pitch();
		float rpm = Vamos_Geometry::rad_s_to_rpm (it->car->engine ()->rotational_speed ());
		sound.SetPitch(sid, rpm / 7000.0f);
		//cout << count << " rpm: " << rpm << endl;
		count++;
		float egain = it->car->engine()->throttle();
		egain = egain*0.5+0.5;
		if (rpm < 500.0)
		{
			egain *= (rpm / 7000.0f);
		}
		//sound.SetGain(sid, 1.0f);
		sound.SetGain(sid, egain);
		VERTEX carpos;
		carpos.x = cm[0];
		carpos.y = cm[2];
		carpos.z = -cm[1];
		//sound.SetPos(sid, carpos);
		VERTEX campos = cam.position;
		campos.Scale(-1);
		//sound.SetListenerPos(campos);
		QUATERNION q = cam.dir;
		VERTEX at, up;
		up.y = 1.0;
		at.z = -1;
		//sound.SetListenerOrientation(q.ReturnConjugate().RotateVec(at), q.ReturnConjugate().RotateVec(up));
		
		VERTEX camvel = cam.position - cam_lastpos;
		camvel.Scale(1.0/dt);
		if (camswitch)
			camvel.zero();
		//sound.SetListenerVel(camvel);
		sound.SetListener(campos, camvel, q.ReturnConjugate().RotateVec(at), q.ReturnConjugate().RotateVec(up));
		
		VERTEX carvel;
		carvel.x = carpos.x - it->car->car_lastpos[0];
		carvel.y = carpos.y - it->car->car_lastpos[1];
		carvel.z = carpos.z - it->car->car_lastpos[2];
		carvel.Scale(1.0/dt);
		//sound.SetVel(sid, carvel);
		if (!carpos.nan() && !carvel.nan())
			sound.SetPosVel(sid, carpos, carvel);
		
		cam_lastpos = cam.position;
		it->car->car_lastpos[0] = cm[0];
		it->car->car_lastpos[1] = cm[2];
		it->car->car_lastpos[2] = -cm[1];
	}
	
	//update info for replay
	//if (replay.Recording >= 0)
	{
		GetCar(CONT_PLAYERLOCAL)->car->GetState(replay.curstate.chassispos, 
			replay.curstate.chassisorientation,
			replay.curstate.chassisvel,
			replay.curstate.chassisangvel,
			replay.curstate.suspdisp,
			replay.curstate.suspcompvel,
			replay.curstate.whlangvel,
			replay.curstate.gear,
			replay.curstate.enginespeed,
			replay.curstate.clutchspeed,
			replay.curstate.enginedrag,
			replay.curstate.tirespeed
			);
		replay.curstate.segment = GetCar(CONT_PLAYERLOCAL)->segment_index;
		replay.curstate.time = replay.GetTime();
		multiplay.GetCurState(0)->CopyFrom(replay.curstate);
		multiplay.GetCurState(0)->time = multiplay.GetTime(0);
	}
	replay.IncrementFrame();
	multiplay.Update(timefactor/fps);
	if (replay.Playing() != -1)
	{
		//int idx = replay.LoadState();
		CARSTATE * cst = replay.LoadState();
		if (cst != NULL)
		{
			//cout << "Keyframe: " << replay.LoadState() << endl;
			Vamos_World::Car_Information * rcar = NULL;
			if (replay.GhostCar())
				rcar = GetCar(CONT_REPLAY);
			else
				rcar = GetCar(CONT_PLAYERLOCAL);
			rcar->segment_index = cst->segment;
			rcar->car->SetState(cst->chassispos, 
				cst->chassisorientation,
				cst->chassisvel,
				cst->chassisangvel,
				cst->suspdisp,
				cst->suspcompvel,
				cst->whlangvel,
				cst->gear,
				cst->enginespeed,
				cst->clutchspeed,
				cst->enginedrag,
				cst->tirespeed
				);
		}
	}
	
	if (multiplay.NumConnected() > 0)
	{
		int p;
		for (p = 0; p < multiplay.NumConnected(); p++)
		{
			if (multiplay.StateToLoad(p+1))
			{
				CARSTATE * cst = multiplay.GetLoadState(p+1);
				if (cst != NULL)
				{
					//world->m_cars[p+1].segment_index = cst->segment;
					GetCar(CONT_PLAYERREMOTE)->segment_index = cst->segment;
					//world->m_cars[p+1].car->SetState(cst->chassispos, 
					GetCar(CONT_PLAYERREMOTE)->car->SetState(cst->chassispos,
					cst->chassisorientation,
					cst->chassisvel,
					cst->chassisangvel,
					cst->suspdisp,
					cst->suspcompvel,
					cst->whlangvel,
					cst->gear,
					cst->enginespeed,
					cst->clutchspeed,
					cst->enginedrag,
					cst->tirespeed
					);
				}
				
				multiplay.ClearStateToLoad(p+1);
			}
		}
	}
	
	/*if ((world->focused_car () != 0) && (mp_sounds != 0))
	{
		double tire_slide = 0.0;
		double grass_speed = 0.0;
		double gravel_speed = 0.0;
		double scrape_speed = 0.0;
		double hard_crash_speed = 0.0;
		double soft_crash_speed = 0.0;
		
		for (std::vector <Vamos_World::Contact_Info>::const_iterator 
			 it = world->m_contact_info.begin ();
		   it != world->m_contact_info.end ();
		   it++)
		{
		  if (it->car != world->focused_car ()->car) continue;
		
		  if ((it->track_material == Material::ASPHALT)
			  || (it->track_material == Material::CONCRETE)
			  || (it->track_material == Material::METAL))
		  {
			if (it->car_material == Material::RUBBER)
			  {
				tire_slide = world->focused_car ()->car->slide ();
			  }
			else if (it->car_material == Material::METAL)
			  {
				scrape_speed = it->parallel_speed;
				hard_crash_speed = it->perpendicular_speed;
			  }
		  }
		  else if (it->track_material == Material::GRASS)
			{
			  grass_speed = it->parallel_speed;
			}
		  else if (it->track_material == Material::GRAVEL)
			{
			  gravel_speed = it->parallel_speed;
			}
		  else if (it->track_material == Material::RUBBER)
			{
			  soft_crash_speed = it->perpendicular_speed;
			}
		}
		
		mp_sounds->play_engine_sound (focused_car ()->car->engine_pitch (),
									focused_car ()->car->engine_volume ());
		
		mp_sounds->play_tire_squeal_sound (tire_slide);
		mp_sounds->play_grass_sound (grass_speed);
		mp_sounds->play_gravel_sound (gravel_speed);
		mp_sounds->play_scrape_sound (scrape_speed);
		mp_sounds->play_wind_sound ((focused_car ()->car->chassis ().
								   cm_velocity () 
								   - mp_atmosphere->velocity ()).abs ());
		mp_sounds->play_hard_crash_sound (hard_crash_speed);
		mp_sounds->play_soft_crash_sound (soft_crash_speed);
		
		mp_sounds->update ();
	}*/
	
	world->m_contact_info.clear ();
}

extern void MainPause();
extern void MainUnpause();

double dabs(double val)
{
	if (val < 0)
		return -val;
	else
		return val;
}

void VAMOSWORLD::ProcessControls(SDL_Joystick ** js, float timefactor, float fps)
{
	//SDL_JoystickUpdate();
	
	int numcontrols = gamecontrols.GetNumControls();
	CONTROL * controls = gamecontrols.GetControls();
	
	string dofunction = "";
	double dovalue = 0.0;
	double dotime = 0.0;
	bool held = false;
	
	if (GetCar(CONT_PLAYERLOCAL) != 0)
	{
		
		int i;
		
		joyinfo_js = js[0];
		joyinfo_jsarray = js;
		
		if (replay.Playing() != -1)
		{
			for (i = 0; i < replay.GetNumFuncs(); i++)
			{
				FUNCTION_MEMORY curfunc = replay.GetFunc(i);
				if (curfunc.active)
				{
					//cout << curfunc.func_name << ", " << curfunc.newval << endl;
					if (replay.GhostCar())
						DoOp(GetCar(CONT_REPLAY), curfunc.func_name, curfunc.newval, 0.0, false, timefactor, fps); 
					else
						DoOp(GetCar(CONT_PLAYERLOCAL), curfunc.func_name, curfunc.newval, 0.0, false, timefactor, fps); 
				}
			}
		}
		
		if (MP_DBGDEEP)
			cout << "multiplay tick start" << endl;
		if (multiplay.NumConnected() > 0)
		{
			//do remote player ops
			//only do them if we're supposed to tick this frame
			if (multiplay.TickCar(1) && !multiplay.NOOPTick(1))
			{
				for (i = 0; i < multiplay.NumFuncs(1); i++)
				{
					FUNCTION_MEMORY curfunc = multiplay.GetFuncMem(1)[i];
					dofunction = curfunc.func_name;
					if (curfunc.active && !(dofunction.find("view_") == 0 || dofunction == "screen_shot" || dofunction == "track_shot" || dofunction == "pause" || dofunction == "reset"))
					{
						//cout << curfunc.func_name << ", " << curfunc.newval << endl;
						DoOp(GetCar(CONT_PLAYERREMOTE), curfunc.func_name, curfunc.newval, 0.0, false, timefactor, fps); 
					}
					dofunction = "";
				}
			}
		}
		if (MP_DBGDEEP)
			cout << "multiplay tick done" << endl;
		
		//if (multiplay.NumConnected() > 0)
			//cout << "gear: " <<	GetCar(CONT_PLAYERREMOTE)->car->transmission()->gear() << endl;
		//cout << "rpm: " << Vamos_Geometry::rad_s_to_rpm (GetCar(CONT_PLAYERREMOTE)->car->engine ()->rotational_speed ()) << endl;
		
		//else
		{
			for (i = 0; i < numcontrols; i++)
			{
				dofunction = "";
				dovalue = 0.0;
				dotime = 0.0;
				held = false;
				
				//joystick input
				if (controls[i].GetType() == Joy)
				{
					SDL_Joystick * joytouse;
					
					int joynum = controls[i].GetJoyNum();
									
					if (joynum >= 0 && joynum < SDL_NumJoysticks())
					{
						joytouse = js[joynum];
						
						if (controls[i].GetJoyType() == Button)
						{
							if (controls[i].GetOneTime())
							{
								if (controls[i].GetJoyPushDown())
								{
									if (!controls[i].GetJoyButtonLastState() && SDL_JoystickGetButton(joytouse, controls[i].GetJoyButton()))
									{
										dofunction = controls[i].GetName();
										dovalue = 1.0;
									}
									/*else if (controls[i].GetJoyButtonLastState() && !SDL_JoystickGetButton(joytouse, controls[i].GetJoyButton()))
									{
										dofunction = controls[i].GetName();
										dovalue = 0.0;
									}*/
								}
								else if (!controls[i].GetJoyPushDown())
								{
									//dofunction = controls[i].GetName();
									//dovalue = 1.0;
									if (!controls[i].GetJoyButtonLastState() && SDL_JoystickGetButton(joytouse, controls[i].GetJoyButton()))
									{
										dofunction = controls[i].GetName();
										dovalue = 0.0;
									}
									/*else if (controls[i].GetJoyButtonLastState() && !SDL_JoystickGetButton(joytouse, controls[i].GetJoyButton()))
									{
										dofunction = controls[i].GetName();
										dovalue = 1.0;
									}*/
								}
							}
							else
							{
								if (SDL_JoystickGetButton(joytouse, controls[i].GetJoyButton()))
								{
									dofunction = controls[i].GetName();
									if (controls[i].GetJoyPushDown())
										dovalue = 1.0;
									else
										dovalue = 0.0;
								}
								else
								{
									dofunction = controls[i].GetName();
									if (controls[i].GetJoyPushDown())
										dovalue = 0.0;
									else
										dovalue = 1.0;
								}
								
								held = true;
							}
							
							controls[i].SetJoyButtonLastState(SDL_JoystickGetButton(joytouse, controls[i].GetJoyButton()));
						}
						else if (controls[i].GetJoyType() == Axis)
						{
							double val = SDL_JoystickGetAxis(joytouse, controls[i].GetJoyAxis());
							val = val / 32768.0;
							if (val >= 0)
								val = val / gamecontrols.GetCalibration(controls[i].GetJoyNum(),controls[i].GetJoyAxis(),true);
							else
							{
								val = val / -gamecontrols.GetCalibration(controls[i].GetJoyNum(),controls[i].GetJoyAxis(),false);
							}
							if (val < -1)
								val = -1;
							if (val > 1)
								val = 1;
							double deadzone = 0.0;
							if (gamecontrols.GetDeadzone() == "low")
								deadzone = 0.05;
							else if (gamecontrols.GetDeadzone() == "med")
								deadzone = 0.1;
							else if (gamecontrols.GetDeadzone() == "high")
								deadzone = 0.2;
							if (dabs(val) < deadzone)
								val = 0;
							else
							{
								if (val < 0)
									val = (val + deadzone)*(1.0/(1.0-deadzone));
								else
									val = (val - deadzone)*(1.0/(1.0-deadzone));
							}
							
							bool crossedover = false;
							if (controls[i].GetJoyAxisPlus())
							{
								if (val < 0)
								{
									crossedover = true;
									val = 0;
								}
							}
							else
							{
								if (val > 0)
								{
									val = 0;
									crossedover = true;
								}
								val = -val;
							}
							
							dovalue = val;
							held = true;
							
							/*if (crossedover && (controls[i].GetName() == "steer_left" || controls[i].GetName() == "steer_right"))
							{
								//special case for steering:  don't let the left & right fight
							}
							else*/
								dofunction = controls[i].GetName();
							
							/*if ((controls[i].GetName() == "steer_left" || controls[i].GetName() == "steer_right"))
							{
								cout << controls[i].GetName() << "," << dofunction << "," << crossedover << "," << val << endl;
							}*/
						}
					}
					else
					{
						//joystick out of range
					}
				}
				
				//keyboard input
				else if (controls[i].GetType() == Key)
				{
					//one time
					if (controls[i].GetOneTime())
					{
						/*for (int k = 0; k < keyman.queuedepth; k++)
						{
							if (keyman.queue[k] == controls[i].GetKeyCode())
							{
								dofunction = controls[i].GetName();
							}
						}*/
						
						if (controls[i].GetKeyPushDown())
						{
							if (keyman.keys[controls[i].GetKeyCode()] && !keyman.lastkeys[controls[i].GetKeyCode()])
							{
								dofunction = controls[i].GetName();
								dovalue = 1.0;
							}
						}
						else
						{
							if (!keyman.keys[controls[i].GetKeyCode()] && keyman.lastkeys[controls[i].GetKeyCode()])
							{
								dofunction = controls[i].GetName();
								dovalue = 1.0;
							}
						}
					}
					else //held
					{
						held = true;
						
						if (controls[i].GetKeyPushDown())
						{
							if (keyman.keys[controls[i].GetKeyCode()])
							{
								dofunction = controls[i].GetName();
								dovalue = 1.0;
							}
							else if (keyman.lastkeys[controls[i].GetKeyCode()])
							{
								dofunction = controls[i].GetName();
								dovalue = 0.0;
							}
						}
						else
						{
							if (!keyman.keys[controls[i].GetKeyCode()])
							{
								dofunction = controls[i].GetName();
								dovalue = 1.0;
							}
							else if (!keyman.lastkeys[controls[i].GetKeyCode()])
							{
								dofunction = controls[i].GetName();
								dovalue = 0.0;
							}
						}
					}
				}
				
				if (dofunction == "steer_left")
				{
					//steer_to (dovalue, timefactor, fps);
					steer_set(dovalue, true);
					//cout << dofunction << "," << dovalue << endl;
					//dovalue = steerpos;
					dofunction = "";
					//cout << dofunction << "," << dovalue << endl;
					//steerleftval = dovalue;
				}
				if (dofunction == "steer_right")
				{
					//steer_to (-dovalue, timefactor, fps);
					steer_set(-dovalue, false);
					//dovalue = steerpos;
					dofunction = "";
					//steerrightval = dovalue;
				}
				
				if (((state.GetGameState() == STATE_PLAYING || dofunction == "gas") && (replay.Playing() == -1 || replay.GhostCar())) || dofunction.find("view_") == 0 || dofunction.find("replay_") == 0 || dofunction == "screen_shot" || dofunction == "track_shot" || dofunction == "pause")
				{
					if (!settings.GetAutoClutch() || dofunction != "engage")
						DoOp(GetCar(CONT_PLAYERLOCAL), dofunction, dovalue, dotime, held, timefactor, fps);
				}
			}
			
			if (state.GetGameState() == STATE_STAGING)
				DoOp(GetCar(CONT_PLAYERLOCAL), "brake", 1.0, 0.0, true, timefactor, fps);
		}
	}
	
	if (!CheckMouseControls())
		steer_commit();
	
	if (settings.GetAutoClutch())
	{
		float autoclutch = 0.0;

		if (GetCar(CONT_PLAYERLOCAL)->car->engine()->rotational_speed() < AUTO_CLUTCH_THRESH)
		{
			float rotspeed = GetCar(CONT_PLAYERLOCAL)->car->engine()->rotational_speed();
			float stall = GetCar(CONT_PLAYERLOCAL)->car->engine()->stall_speed() + AUTO_CLUTCH_MARGIN;
			float clutch = (rotspeed-stall) / (AUTO_CLUTCH_THRESH-stall);
			if (clutch < 0)
				clutch = 0;
			clutch = 1.0f - clutch;
			//cout << "Autoclutch: " << clutch << endl;
			//DoOp(GetCar(CONT_PLAYERLOCAL), "clutch", clutch, 0.0, true, timefactor, fps);
			//DoOp(GetCar(CONT_PLAYERLOCAL), "clutch", 1.0, 0.0, true, timefactor, fps);
			//DoOp(GetCar(CONT_PLAYERLOCAL), "neutral", 1.0, 0.0, false, timefactor, fps);
			if (clutch > autoclutch)
				autoclutch = clutch;
		}
		
		if (shift_time > 0)
		{
			float clutch = shift_time / AUTO_CLUTCH_ENGAGE_TIME;
			if (clutch > autoclutch)
				autoclutch = clutch;
			
			//DoOp(GetCar(CONT_PLAYERLOCAL), "clutch", 0.0, 0.0, true, timefactor, fps);
			shift_time -= timefactor/fps;
		}
		
		if (!GetCar(CONT_PLAYERLOCAL)->car->ShiftPending())
		{
			DoOp(GetCar(CONT_PLAYERLOCAL), "clutch", autoclutch, 0.0, true, timefactor, fps);
		}
		//else
			//cout << "Shifting..." << endl;
	}
}

void VAMOSWORLD::DoOp(Vamos_World::Car_Information * c, string dofunction, float dovalue, float dotime, bool held, float timefactor, float fps)
{
	//do the requested operation
	if (dofunction != "")
	{
		if (dofunction == "reset")
			world->reset(c);
		else if (dofunction == "brake")	c->car->brake (dovalue, dotime);
		else if (dofunction == "gas") c->car->gas (dovalue, dotime);
		else if (dofunction == "clutch") c->car->clutch (1.0f-dovalue, dotime);
		else if (dofunction == "steer_left" || dofunction == "steer_right")
		{
			/*if (propsteer)
			{
				//Vamos_Geometry::Three_Vector vel = world->focused_car ()->car->chassis()->cm_velocity();
				//float speed = vel.abs();
			}
			
			if (dofunction == "steer_left")
			{
				//steer_to (dovalue, timefactor, fps); 
				steer_set(dovalue, true);
				//steerleftval = dovalue;
			}
			if (dofunction == "steer_right")
			{
				//steer_to (-dovalue, timefactor, fps);
				steer_set(-dovalue, false);
				//steerrightval = dovalue;
			}*/
			//world->focused_car ()->car->steer (steerpos, dotime);
			c->car->steer (dovalue, dotime);
			
			//if (dofunction == "steer_left") world->focused_car ()->car->steer (dovalue, dotime);
			//if (dofunction == "steer_right") world->focused_car ()->car->steer (-dovalue, dotime);
		}
		else if (dofunction == "disengage_shift_up") 
		{
			c->car->disengage_clutch (0.2);
			c->car->shift_up ();
			if (c == GetCar(CONT_PLAYERLOCAL))
			{
				shift_time = AUTO_CLUTCH_ENGAGE_TIME;
			}
		}
		else if (dofunction == "disengage_shift_down") 
		{
			c->car->disengage_clutch (0.2);
			c->car->shift_down ();
			if (c == GetCar(CONT_PLAYERLOCAL))
			{
				shift_time = AUTO_CLUTCH_ENGAGE_TIME;
			}
		}
		else if (dofunction == "engage") 
		{
			if (c->car->last_gear () == 0)
				c->car->engage_clutch (1.0);
			else
				c->car->engage_clutch (0.2);
		}
		else if (dofunction == "neutral") 
		{
			c->car->shift(0);
		}
		else if (dofunction == "start_engine") c->car->start_engine ();
		
		else if (dofunction == "view_chaserigid") {mq1.AddMessage("Camera set to rigid chase mode");SetCameraMode(CMChaseRigid);}
		else if (dofunction == "view_chase") {mq1.AddMessage("Camera set to flexible chase mode");SetCameraMode(CMChase);}
		else if (dofunction == "view_orbit") {mq1.AddMessage("Camera set to mouse orbit mode");SetCameraMode(CMOrbit);}
		else if (dofunction == "view_hood") {mq1.AddMessage("Camera set to hood mounted mode");SetCameraMode(CMHood);}
		else if (dofunction == "view_free") {mq1.AddMessage("Camera set to free mode");SetCameraMode(CMFree);}
		else if (dofunction == "view_incar") {mq1.AddMessage("Camera set to in-car mode");SetCameraMode(CMInCar);}
		else if (dofunction == "view_external") {mq1.AddMessage("Camera set to external fixed mode");SetCameraMode(CMExternal);}
		else if (dofunction == "view_extfollow") {mq1.AddMessage("Camera set to external follow mode");SetCameraMode(CMExtFollow);}
		else if (dofunction == "joystick_info") {joyinfo = !joyinfo;}
		else if (dofunction == "proportionalsteer_toggle") propsteer = !propsteer;
		else if (dofunction == "track_shot") build_track_shot();
		else if (dofunction == "screen_shot") snap_screenshot();
		else if (dofunction == "replay_ff") replay.Jump(replay.GetTime()+10);
		else if (dofunction == "replay_rw") {double t = replay.GetTime()-10; if (t < 0) t = 0; replay.Jump(t);}
		else if (dofunction == "pause")
		{
			if (timefactor != 0) 
				MainPause();
				//timefactor = 0;
			//else
				//timefactor = 1.0;
				//MainUnpause(); //this never gets called; with timefactor == 0, vamosworld.Update() doesn't get called
			//cout << "pause" << endl;
		}
		//else if (dofunction == "menu") menu.MainMenu();
		else
		{
			//no such function!
			//cout << dofunction << endl;
			if (verbose_output)
				cout << "No such function: " << dofunction << endl;
		}
		
		//record the control input for replay
		//if (dofunction != "")
		{
			int state = 0;
			if (held)
				state = 1;
			if (replay.Playing() == -1 && !(dofunction.find("view_") == 0 || dofunction == "screen_shot" || dofunction == "track_shot" || dofunction == "pause"))
			{
				if (c->car->get_controller() == CONT_PLAYERLOCAL)
					replay.AddRecord(dofunction, dovalue, state);
			}
			
			if (MP_DBGDEEP)
				cout << "multiplay add record start" << endl;			
			if (multiplay.NumConnected() > 0 && !(dofunction.find("view_") == 0 || dofunction == "screen_shot" || dofunction == "track_shot" || dofunction == "pause" || dofunction == "reset"))
			{
				if (c->car->get_controller() == CONT_PLAYERLOCAL)
					multiplay.AddRecord(dofunction, dovalue, state);
			}
			if (MP_DBGDEEP)
				cout << "multiplay add record done" << endl;
		}
		
		//cout << dofunction << endl;
	}
}


void VAMOSWORLD::DrawDigitalHUD()
{
	
}


void VAMOSWORLD::DrawHUD()
{
	char tempchar[8];
	
	float w = 0.25;
	float h = 0.333;
	float y = 0.65;
	float x = 0.05;
	utility.Draw2D(x, y, x+w, y+h, tachbase);
	
	float maxrpm = Vamos_Geometry::rad_s_to_rpm (GetCar(CONT_PLAYERLOCAL)->car->engine ()->max_rotational_speed ());
	float tachspace = 0.55;
	float curtachrot = -7.0f*tachspace;
	float currpm = 11000;
	
	//draw RPM band
	while (currpm >= 0)
	{
		if (maxrpm > currpm+500)
			utility.Draw2D(x, y, x+w, y+h, tachband, curtachrot);
		curtachrot += tachspace;
		currpm -= 1000;
	}
	
	float nscale = 0.00055;
	
	//draw red band
	float redline = Vamos_Geometry::rad_s_to_rpm (GetCar(CONT_PLAYERLOCAL)->car->engine ()->peak_engine_speed ());
	redline -= 5000;
	utility.Draw2D(x, y, x+w, y+h, tachredband, -redline*nscale);
	
	//draw text
	currpm = 11000;
	curtachrot = -7.0f*tachspace;
	while (currpm >= 0)
	{
		//if (maxrpm > currpm+100)
		if (maxrpm >= currpm-500)
		{
			float aspect = w / h;
			QUATERNION rot;
			//curtachrot = 0;
			rot.Rotate(curtachrot, 0,0,1);
			VERTEX textoffset;
			textoffset.Set(0,-0.125,0);
			textoffset = rot.RotateVec(textoffset);
			textoffset.x *= -aspect;
			//textoffset.DebugPrint();
			VERTEX tachcenter;
			tachcenter.Set(x+w/2.0,y+h/2.0,0);
			//tachcenter.DebugPrint();
			textoffset = textoffset + tachcenter;
			VERTEX offset;
			offset.Set(-0.01*aspect, -0.026, 0);
			textoffset = textoffset + offset;
			char tc[8];
			sprintf(tc,"%i", (int)(currpm / 1000.0));
			font.Print(textoffset.x, textoffset.y, tc, 1, 6, 0,0,0);
			//utility.Draw2D(x, y, x+w, y+h, tachband, curtachrot);
		}
		curtachrot += tachspace;
		currpm -= 1000;
	}
	
	utility.Draw2D(1.0-x-w, y, 1.0-x, y+h, speedo);
	
	float rpm = Vamos_Geometry::rad_s_to_rpm (GetCar(CONT_PLAYERLOCAL)->car->engine ()->rotational_speed ());
	//cout << rpm << endl;
	
	//float nscale = 0.0005235883;
	//float offset = 300.0f;
	
	//rpm = 0;
	rpm *= nscale;
	//cout << rpm << endl;
	utility.Draw2D(x, y, x+w, y+h, needle, -rpm);
	
	//float mph = Vamos_Geometry::m_s_to_km_h (GetCar(CONT_PLAYERLOCAL)->car->wheel (1)->speed ())*0.621371192;
	float kph = Vamos_Geometry::m_s_to_km_h (GetCar(CONT_PLAYERLOCAL)->car->chassis().cm_velocity().magnitude());
	if (kph < 0)
		kph = -kph;

	float mph = kph * 0.621371192;
	utility.Draw2D(1.0-x-w, y, 1.0-x, y+h, needle, -mph*0.034+0.0);

	char tchar[256];

	if( settings.GetMPH() )
	{
		sprintf(tchar, "%03.0f", mph);
		font.Print(1.0-x-w+0.097, y+0.25, tchar, 0, 7, 1,0,0);
		font.Print(1.0-x-w+0.103, y+0.29, "MPH", 0, 6, 0,0,0);
	}
	else
	{
		sprintf(tchar, "%03.0f", kph);
		font.Print(1.0-x-w+0.097, y+0.25, tchar, 0, 7, 1,0,0);
		font.Print(1.0-x-w+0.103, y+0.29, "KPH", 0, 6, 0,0,0);
	}
	
	int gear = GetCar(CONT_PLAYERLOCAL)->car->transmission ()->gear ();
	if (gear == 0)
		strcpy (tempchar, "N");
	else if (gear < 0)
		strcpy (tempchar, "R");
	else
		sprintf(tempchar, "%i", gear);
	font.Print(x+w/2.0-0.021,y+h-0.14, tempchar, 0, 9, 1,0,0);
	//font.Print(x+w/2.0-0.1,y+h-0.1, tempchar, 1, 8, 1);
	
	float pfuel = GetCar(CONT_PLAYERLOCAL)->car->fuel_tank()->fuelpercent();
	//cout << pfuel << endl;
	pfuel = 0.1*pfuel;
	//utility.Draw2D(0,0.97,pfuel,1, fuelgauge);
	utility.Draw2D(0,0.87,0.1,1, fgbox);
	w = .125;
	h = .1725;
	x = -.037;
	y = 0.905;
	//utility.Draw2D(x,y,x+w,h+y, needle, pfuel*20.0+2.6); //-0.025,0.87,0.1,1.0425
	utility.Draw2D(x,y,x+w,h+y, needle, pfuel*20.0+2.42);
	//font.Print(0.0, 0.9, "Fuel", 1, 7, 0.7, 0, 0, 0.5);
	//font.Print(0.0, 0.92, "Fuel", 1, 7, 1,1,1, 0.5);
	
	float recspace = replay.Recording();
	if (recspace >= 0)
	{
		sprintf(tempchar, "Recording: %2.0f%% full", (recspace) * 100.0f);
		font.Print(0.4, 0.95, tempchar, 1, 6, 1, 0, 0, 1);
	}
	float playspace = replay.Playing();
	if (playspace >= 0)
	{
		sprintf(tempchar, "Playing: %2.0f%% complete", (playspace) * 100.0f);
		font.Print(0.4, 0.95, tempchar, 1, 6, 1, 0, 0, 1);
	}
	
	if (multiplay.NumConnected() > 0)
	{
		/*if (multiplay.GetLatency(1) == 0)
			sprintf(tempchar, "Sync: in sync");
		else
			sprintf(tempchar, "Sync: %4.0f ms", multiplay.GetLatency(1) * 1000.0f);
		font.Print(0.4, 0.89, tempchar, 1, 6, 1, 0, 0, 1);*/
		
		sprintf(tempchar, "Tx: %2.3fkb/s", multiplay.GetTxRate()/1000.0);
		font.Print(0.4, 0.92, tempchar, 1, 6, 1, 0, 0, 1);
		
		sprintf(tempchar, "Rx: %2.3fkb/s", multiplay.GetRxRate()/1000.0);
		font.Print(0.4, 0.95, tempchar, 1, 6, 1, 0, 0, 1);
	}
	
	//draw the mouse control indicators
	float mx, my;
	bool cl, cr;
	bool msteer = mouse.GetMouseControls(&mx, &my, &cl, &cr);
	if (msteer && (replay.Playing() == -1 || replay.GhostCar()))
	{
		mx = -mx;
		float dx, dy;
		dx = 0.4;
		dy = 0.8;
		float dw = 0.2;
		float dh = 0.2;
		dh *= 1.33333;
		utility.Draw2D(dx, dy, dx+dw, dy+dh, m_sliderh);
		float ballscale = 4.0;
		utility.Draw2D((dx+dw/2.0)-dw/(ballscale*2.0)+mx*dw/2.7, (dy+dh/2.0)-dh/(ballscale*2.0), (dx+dw/2.0)-dw/(ballscale*2.0)+mx*dw/2.7+dw/ballscale, (dy+dh/2.0)-dh/(ballscale*2.0)+dh/ballscale, m_ballh);
		
		my = -my;
		dx = 0.3;
		dy = 0.87;
		dw = 0.1;
		dh = 0.1;
		dh *= 1.33333;
		ballscale = 1.0;
		utility.Draw2D(dx, dy, dx+dw, dy+dh, m_sliderv);
		utility.Draw2D((dx+dw/2.0)-dw/(ballscale*2.0), (dy+dh/2.0)-dh/(ballscale*2.0)+my*dh/2.7, (dx+dw/2.0)-dw/(ballscale*2.0)+dw/ballscale, (dy+dh/2.0)-dh/(ballscale*2.0)+my*dh/2.7+dh/ballscale, m_ballv);
	}
}

void VAMOSWORLD::reset()
{
	world->reset(true);
}

void VAMOSWORLD::DisplayJoyInfo()
{
    int joynum = 0;
    
	string joystring = "";
    char tchar[256];
    
	float xpos = 0.01;
	
	for (joynum = 0; joynum < SDL_NumJoysticks(); joynum++)
	{	
		if (SDL_NumJoysticks() > joynum)
		{
			int i;
			for (i = 0; i < SDL_JoystickNumAxes(joyinfo_jsarray[joynum]); i++)
			{
					sprintf(tchar, "Axis %i: %f\n", i, (float)SDL_JoystickGetAxis(joyinfo_jsarray[joynum], i)/32768.0f);
					joystring = joystring + tchar;
			}
			
			for (i = 0; i < SDL_JoystickNumButtons(joyinfo_jsarray[joynum]); i++)
			{
					sprintf(tchar, "Button %i: %d\n", i, SDL_JoystickGetButton(joyinfo_jsarray[joynum], i));
					joystring = joystring + tchar;
			}
			
			font.Print(xpos,0.11, joystring.c_str(), 1, 5, 1,1,1);
			xpos += 0.2;
			joystring = "";
		}
	}
	
    /*string joystring = "";
    char tchar[256];
    
    if (SDL_NumJoysticks() > 0)
    {
        int i;
        for (i = 0; i < SDL_JoystickNumAxes(joyinfo_js); i++)
        {
                sprintf(tchar, "Axis %i: %f\n", i, (float)SDL_JoystickGetAxis(joyinfo_js, i)/32768.0f);
                joystring = joystring + tchar;
        }
        
        for (i = 0; i < SDL_JoystickNumButtons(joyinfo_js); i++)
        {
                sprintf(tchar, "Button %i: %d\n", i, SDL_JoystickGetButton(joyinfo_js, i));
                joystring = joystring + tchar;
        }
    }
    
    font.Print(0.01,0.11, joystring.c_str(), 1, 5, 1,1,1);*/
}

void VAMOSWORLD::build_track_shot()
{
	int outsizex = 512;
	int outsizez = 512;
	
	//outsizex = 128;
	//outsizez = 128;
	
	Uint32 rmask, gmask, bmask, amask;

	/* SDL interprets each pixel as a 32-bit number, so our masks must depend
	   on the endianness (byte order) of the machine */
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
	
	SDL_Surface *out = SDL_CreateRGBSurface(SDL_SWSURFACE, outsizex, outsizez, 32, rmask, gmask, bmask, amask);
	
	//build a mask of the track
	Vamos_Geometry::Three_Vector wp;
	wp[0] = 0;
	wp[1] = 0;
	wp[2] = 0;
//	size_t segid;
	
	SDL_LockSurface(out);
	
	int i, n;
	for (n = 0; n < outsizez; n++)
	{
		for (i = 0; i < outsizex; i++)
		{
			float xp = (float) i / (float) outsizex;
			float yp = (float) n / (float) outsizez;
			
			//wp[0] = xp * terrain.GetSize().x + terrain.GetOffset().x;
			//wp[1] = yp * terrain.GetSize().z + terrain.GetOffset().z;
			wp[0] = yp * terrain.GetSize().x + terrain.GetOffset().x;
			wp[1] = -(xp * terrain.GetSize().z + terrain.GetOffset().z);
			//wp[2] = yp * terrain.GetSize().z + terrain.GetOffset().z;
			
			int val = track_ptr->ontrack(wp);
			//cout << track_ptr->ontrack(wp);
			//size_t sg = 0;
			//cout << track_ptr->distance(wp, sg) << ",";
			//cout << track_ptr->elevation(wp, 0, sg) << ",";
			//cout << wp[0] << "," << wp[1] << endl;
			
			((unsigned char *) out->pixels)[(i*outsizex+n)*4] = val * 250;
			((unsigned char *) out->pixels)[(i*outsizex+n)*4+1] = val * 250;
			((unsigned char *) out->pixels)[(i*outsizex+n)*4+2] = val * 250;
			((unsigned char *) out->pixels)[(i*outsizex+n)*4+3] = 250;
		}
		
		cout << "Writing line " << n << endl;
	}
	
	SDL_UnlockSurface(out);
	
	//terrain.GetOffset().DebugPrint();
	//terrain.GetSize().DebugPrint();
	
	SDL_SaveBMP(out, (settings.GetSettingsDir() + "/screenshots/trackmask.bmp").c_str());
	SDL_FreeSurface(out);
}

void VAMOSWORLD::steer_to(float val, float timefactor, float fps)
{
	//steerpos = val;
	//steerpos += val*timefactor/fps;
	
	bool inhibit = (gamecontrols.GetJoystickType() == "wheel");
	
	if (gamecontrols.GetCompensation() == "low")
	{
		if (val < 0)
			val = -val*val;
		else
			val = val*val;
	}
	else if (gamecontrols.GetCompensation() == "med")
	{
		val = val*val*val;
	}
	else if (gamecontrols.GetCompensation() == "high")
	{
		if (val < 0)
			val = -val*val*val*val;
		else
			val = val*val*val*val;
	}
	else if (gamecontrols.GetCompensation() == "900to200")
	{
		float decimate = 4.5;
		
		float carmph = Vamos_Geometry::m_s_to_km_h (GetCar(CONT_PLAYERLOCAL)->car->wheel (1)->speed ())*0.621371192;
		
		float normalat = 30;
		float transat = 15;
		
		if (carmph < transat)
			decimate = 1.0;
		else if (carmph < normalat)
		{
			float coeff = (carmph - transat)/(normalat - transat);
			decimate = (decimate-1.0f)*coeff + 1.0f;
		}
		
		val = val/decimate;
	}
	
	float steerstep = 2.0*timefactor/fps;
	
	if (val > steerpos)
	{
		if (val - steerpos <= steerstep)
			steerpos = val;
		else
			steerpos += steerstep;
	}
	else
	{
		if (steerpos - val <= steerstep)
			steerpos = val;
		else
			steerpos -= steerstep;
	}
	
	if (inhibit)
		steerpos = val;
	
	if (steerpos > 1.0)
		steerpos = 1.0;
	if (steerpos < -1.0)
		steerpos = -1.0;
		
	
	
	//cout << val << endl;
}

void VAMOSWORLD::steer_set(float val, bool left)
{
	if (left)
		steervals[0] = val;
	else
		steervals[1] = val;
}

void VAMOSWORLD::steer_commit()
{
	if (steervals[0] != 0)
	{
		steer_to(steervals[0], l_timefactor, l_fps);
	}
	else
	{
		steer_to(steervals[1], l_timefactor, l_fps);
	}
	
	steervals[0] = 0.0;
	steervals[1] = 0.0;
	
	if (replay.Playing() == -1 || replay.GhostCar())
		DoOp(GetCar(CONT_PLAYERLOCAL), "steer_left", steerpos, 0.0, true, l_timefactor, l_fps);
}

void VAMOSWORLD::SetCameraMode(CameraMode newmode)
{
	cammode = newmode;
	ofstream cf;
	/*cf.open("lists/cammode");
	if (cf)
		cf << cammode << endl;
	cf.close();*/
	//CMChaseRigid, CMChase, CMOrbit, CMHood, CMFree };
	
	settings.SetCameraMode(cammode);

	if (newmode == CMFree)
		keyman.freecam = true;
	else
		keyman.freecam = false;
	
	if (newmode == CMExternal || newmode == CMExtFollow)
	{
		lastcamchange = 0.0f;
		camneedupdate = true;
	}
	if (newmode == CMInCar)
	{
		cam_lastpos = cam.position;
		cam_lastpos2 = cam.position;
		cam_lastvel.zero();
		cam_lastaccel.zero();
		cam_jerk.zero();
		camneedupdate = true;
	}
}

Vamos_Body::Car * VAMOSWORLD::GetPlayerCar()
{
	//return world->focused_car()->car;//->fuel_tank()->fuel();
	return GetCar(CONT_PLAYERLOCAL)->car;
}

void VAMOSWORLD::FuelPlayerCar()
{
	GetCar(CONT_PLAYERLOCAL)->car->fuel_tank()->fill();
}

void VAMOSWORLD::clear_cars()
{
	for (std::vector <Vamos_World::Car_Information>::iterator it = world->m_cars.begin ();
	   it != world->m_cars.end ();
	   it++)
	{
		//cout << "deleting car..." << endl;
		delete it->car;
		//cout << "done" << endl;
	}
	world->m_cars.clear();
	//cout << "cars cleared" << endl;
}

void VAMOSWORLD::Trim(float frametime, SDL_Joystick ** js)
{
	int i;
	
	//double accel = 8.0;
	//double accel = 1.0;
	
	//frametime *= accel;
	
	/*Vamos_Body::Car * pcar = GetPlayerCar();
	
	Vamos_Geometry::Three_Vector oldpos = pcar->chassis().cm_position();
	Vamos_Geometry::Three_Vector oldvel = pcar->chassis().cm_velocity();*/
	
	//double trimthresh = 0.00001;
	//double trimthresh = 0.0001;
	//double trimthresh = 0.05*frametime;
	//double trimthresh = 0.0005*frametime;
	
	for (std::vector <Vamos_World::Car_Information>::iterator it = world->m_cars.begin ();
		it != world->m_cars.end ();
		it++)
	{
		it->car->brake (1.0, 0.0);
	}
	
	for (i = 0; i < MAX_TRIM_LOOPS; i++)
	{
		if (i % 500 == 0)
		{
			char lt[256];
			sprintf(lt, "Loading...\nTrimming: %2.0f%%", ((float) i)/MAX_TRIM_LOOPS*100.0);
			LoadingScreen(lt);
		}
		
		/*for (std::vector <Vamos_World::Car_Information>::iterator it = world->m_cars.begin ();
		it != world->m_cars.end ();
		it++)
		{
			VERTEX pos;
			Vamos_Geometry::Three_Vector cp = it->car->chassis().position();
			pos.Set(cp[0], cp[1], cp[2]);
			pos.DebugPrint();
		}
		cout << endl;*/
		
		PhysUpdate(frametime);
	
		for (std::vector <Vamos_World::Car_Information>::iterator it = world->m_cars.begin ();
		it != world->m_cars.end ();
		it++)
		{
			/*VERTEX pos;
			Vamos_Geometry::Three_Vector cp = it->car->chassis().position();
			pos.Set(cp[0], cp[1], cp[2]);
			pos.DebugPrint();*/
						
			//Update(1.0, 1.0f/frametime, js);
			
			{
				const double distance = 
				world->mp_track->distance (it->car->chassis ().position (), it->segment_index);
				const int sector = world->mp_track->sector (distance);
				//it->timing.update (mp_timer->get_time_step (), distance, sector);
				it->timing.update (frametime, distance, sector);
			}
			
			/*const double distance = 
				world->mp_track->distance (GetCar(CONT_PLAYERLOCAL)->car->chassis ().position (), GetCar(CONT_PLAYERLOCAL)->segment_index);
			const int sector = world->mp_track->sector (distance);
			//it->timing.update (mp_timer->get_time_step (), distance, sector);
			GetCar(CONT_PLAYERLOCAL)->timing.update (frametime, distance, sector);*/
		}
		
		world->m_contact_info.clear ();
	}
	
	//cout << i << endl;
	
	/*trimthresh = 0.00005;
	for (i = 0; i < MAX_TRIM_LOOPS; i++)
	{
		PhysUpdate(frametime);
		for (std::vector <Vamos_World::Car_Information>::iterator it = world->m_cars.begin ();
		it != world->m_cars.end ();
		it++)
		{
			const double distance = 
			world->mp_track->distance (it->car->chassis ().position (), it->segment_index);
			const int sector = world->mp_track->sector (distance);
			//it->timing.update (mp_timer->get_time_step (), distance, sector);
			it->timing.update (frametime, distance, sector);
		}
		
		world->m_contact_info.clear ();
		
		//if (oldpos == pcar->chassis().cm_position() && oldvel == pcar->chassis().cm_velocity())
		if ((oldpos - pcar->chassis().cm_position()).abs() <  trimthresh && (oldvel - pcar->chassis().cm_velocity()).abs() < trimthresh)
		{
			//cout << "trimmed" << endl;
			break;
		}
		
		oldpos = pcar->chassis().cm_position();
		oldvel = pcar->chassis().cm_velocity();
	}
	
	cout << i << endl;*/
	
	//GetCar(CONT_PLAYERLOCAL)->car->brake (0.0, 0.0);
	
	for (std::vector <Vamos_World::Car_Information>::iterator it = world->m_cars.begin ();
		it != world->m_cars.end ();
		it++)
	{
		it->car->brake (0.0, 0.0);
	}
}

void VAMOSWORLD::PhysUpdate(float dt)
{
	int count = 0;
	
	for (std::vector <Vamos_World::Car_Information>::iterator it = world->m_cars.begin ();
			   it != world->m_cars.end ();
			   it++)
	{
		if (count == 0 || multiplay.TickCar(count) || (count == 1 && replay.GhostCar()))
		{
			it->car->propagate (dt);
			world->interact (it->car, it->segment_index);
		}
		
		count++;

	  /*for (std::vector <Vamos_World::Car_Information>::iterator 
			 other = world->m_cars.begin ();
		   other != world->m_cars.end ();
		   other++)
		{
		  if (other != it)
			{
			  world->collide (it->car, other->car);
			}
		}*/
	}
}

void VAMOSWORLD::ToggleHUD()
{
	display_hud = !display_hud;
	settings.SetHUDEnabled(display_hud);
}

Vamos_World::Car_Information * VAMOSWORLD::GetCar(CONTROLLERTYPE p)
{
	for (std::vector <Vamos_World::Car_Information>::iterator it = world->m_cars.begin ();
			   it != world->m_cars.end ();
			   it++)
	{
		if (it->car->get_controller() == p)
		{
			return &(*it);
		}
	}
	
	cout << "Error -- no player car!  Get ready for a crash...." << endl;
	
	return NULL;
}

bool VAMOSWORLD::CheckMouseControls()
{
	float x, y;
	bool cl, cr;
	bool msteer = mouse.GetMouseControls(&x, &y, &cl, &cr);
	if (msteer && (replay.Playing() == -1 || replay.GhostCar()))
	{
		DoOp(GetCar(CONT_PLAYERLOCAL), "steer_left", x, 0.0, true, l_timefactor, l_fps);
		
		float throttle = y;
		float brake = y;
		if (throttle < 0)
			throttle = 0;
		if (brake > 0)
			brake = 0;
		brake = -brake;
		DoOp(GetCar(CONT_PLAYERLOCAL), "gas", throttle, 0.0, true, l_timefactor, l_fps);
		DoOp(GetCar(CONT_PLAYERLOCAL), "brake", brake, 0.0, true, l_timefactor, l_fps);
		
		if (cr)
		{
			DoOp(GetCar(CONT_PLAYERLOCAL), "disengage_shift_up", 1.0, 0.0, true, l_timefactor, l_fps);
		}
		
		if (cl)
		{
			DoOp(GetCar(CONT_PLAYERLOCAL), "disengage_shift_down", 1.0, 0.0, true, l_timefactor, l_fps);
		}
		
		if (cl || cr) 
		{
			DoOp(GetCar(CONT_PLAYERLOCAL), "engage", 1.0, 0.0, true, l_timefactor, l_fps);
		}
	}
	return msteer;
}

void VAMOSWORLD::DrawCars()
{
	glPushMatrix();
	
	QUATERNION goofyfoot;
	goofyfoot.Rotate(-3.141593/2.0, 1,0,0);
	double tempmat[16];
	goofyfoot.GetMat(tempmat);
	glMultMatrixd(tempmat);
	
	
	
	
	float lp[4];
	//lp[0] = -LightPosition[0];
	//lp[1] = LightPosition[2];
	//lp[2] = LightPosition[1];
	lp[0] = LightPosition[0];
	lp[1] = LightPosition[1];
	lp[2] = LightPosition[2];
	lp[3] = 0;
	VERTEX lpv;
	lpv.Set(lp);
	lpv = goofyfoot.ReturnConjugate().RotateVec(lpv);
	lp[0] = lpv.x;
	lp[1] = lpv.y;
	lp[2] = lpv.z;
	glLightfv( GL_LIGHT1, GL_POSITION,  lp);
	
	if (MP_DBGDEEP)
		cout << "draw cars start" << endl;
	//glEnable(GL_STENCIL_TEST);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//glStencilFunc(GL_ALWAYS, 1, ~0);
	draw_cars(false, true);
	if (MP_DBGDEEP)
		cout << "draw cars done" << endl;
	
	glPopMatrix();
}

void VAMOSWORLD::DrawTopLevel()
{
	glPushMatrix();
	
	glDisable(GL_STENCIL_TEST);
	particle.Draw();

	if (display_hud)
	{
		DrawHUD();
		timer.Draw();
	}
	
	if (joyinfo)
	 DisplayJoyInfo();
	
	glPopMatrix();
}
