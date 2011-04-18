#include "objects.h"

#include <algorithm>

OBJECTS::OBJECTS()
{
	error_log.open((settings.GetSettingsDir() + "/logs/objects.log").c_str());
	
	object_list = NULL;
	model_list = NULL;
	loadjpk = false;
}

OBJECTS::~OBJECTS()
{
	error_log.close();
	
	//deallocate objects and models
	DeleteAll();
}

void OBJECTS::DeleteAll()
{
	for (map <string, GLuint>::iterator i = texture_db.begin(); i != texture_db.end(); i++)
	{
		glDeleteTextures(1, &(i->second));
	}
	
	texture_db.clear();
	
	while (object_list != NULL)
		delobject();
	
	while (model_list != NULL)
		delmodel();
		
	if (loadjpk)
		jpk.ClosePack();
}

void OBJECTS::delobject()
{
	if (object_list == NULL)
		return;
	
	//save old tree
	OBJECTNODE * old = object_list;
	object_list = object_list->next;
	delete old;
}

void OBJECTS::delmodel()
{
	if (model_list == NULL)
		return;
	
	//save old tree
	OBJECTMODEL * old = model_list;
	model_list = model_list->next;
	delete old;
}

void OBJECTS::DrawObject(OBJECTNODE * object)
{	
	/*
	int i;
	
	float yoffset = 0.0f;
	float dx, dy, dz, rc;
	
	//range cull
	dx=object->pos.x+cam.position.x; dy=object->pos.y+cam.position.y; dz=object->pos.z+cam.position.z;
    rc=dx*dx+dy*dy+dz*dz;
	
	//cout << dx << "," << dy << "," << dz << ":  " << rc << endl;
	
	//if (rc > lod_far*10)
	//	return;*/

	//use different techniques based on range
	//frustum cull!
	float bound, rd;
	//if (rc > lod_far)
	if ((object->pos + cam.position).len() >= object->model->jmodel.GetRadius())
	{
		//cout << "outside object" << endl;
		//bound = spread*2.0f;
		bound = object->model->jmodel.GetRadius();//*2.0f;
		int i;
		for (i=0;i<6;i++) 
		{
			rd=cam.frustum[i][0]*object->pos.x+
			   cam.frustum[i][1]*object->pos.y+
			   cam.frustum[i][2]*object->pos.z+
			   cam.frustum[i][3];
			if (rd<=-bound && !object->model->skybox)
			{
				return;
			}
		}
	}
	//else cout << "inside object" << endl;
	
	if (object->model->skybox)
	{
		glPushMatrix();
		/*glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glLoadIdentity( );
		gluPerspective( 45.0f, (float)display_x / (float)display_y, 0.1f, 10000.0 );
		glMatrixMode( GL_MODELVIEW );*/
		GLdouble temp_matrix[16];
		cam.PutTransformInto(temp_matrix);
		glLoadMatrixd(temp_matrix);
		glDepthMask(0);
	}
	else
	{
		glPushMatrix();
		GLfloat transform_matrix[16];
		object->dir.GetMat(transform_matrix);
		
		glTranslatef(object->pos.x, object->pos.y, object->pos.z);
		glMultMatrixf(transform_matrix);
	}
	
	glRotated(-90, 1,0,0);
	
	
	if (object->model != NULL)
	{
		if (object->model->fullbright)
			glDisable(GL_LIGHTING);
		else
			glEnable(GL_LIGHTING);
		
		//glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_BLEND);
		//glDisable(GL_DEPTH_TEST);
		glAlphaFunc(GL_GREATER, 0.9f);
		//glAlphaFunc(GL_GEQUAL, 1.0f);
		glEnable(GL_ALPHA_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glCullFace(GL_FRONT);
		//glEnable(GL_CULL_FACE);
		//glDepthMask(0);
		glColor4f(1,1,1,1);
		
		//draw filled & proper polygon
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		object->model->jmodel.DrawStatic();
		
		//draw verts
		if (!object->model->skybox)
		{
			glDisable(GL_ALPHA_TEST);
			//glDisable(GL_DEPTH_TEST);
			glDisable(GL_TEXTURE_2D);
			glPointSize(4.0);
			glDisable(GL_LIGHTING);
			glColor4f(0,0,1,1);
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			object->model->jmodel.DrawStatic(false);
			
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_DEPTH_TEST);
			glPointSize(1.0);
			glColor4f(0,0,1,1);
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			object->model->jmodel.DrawStatic(false);
		}
		
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glColor4f(1,1,1,1);
		
		//glPopAttrib();
	}
	else
		cout << "NULL model" << endl;
	
	if (object->model->skybox)
	{
		glDepthMask(1);
		/*glMatrixMode( GL_PROJECTION );
		glPopMatrix();*/
		glMatrixMode( GL_MODELVIEW );
		glPopMatrix();
	}
	else
		glPopMatrix();
}

void OBJECTS::Draw()
{
	//setup gl flags
	//glEnable(GL_BLEND);
	//glDisable( GL_TEXTURE_2D );
	/*glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,treetex[0]);
	glDisable( GL_LIGHTING);
	glAlphaFunc(GL_GREATER, 0.9f);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );*/
	//glBlendFunc( GL_ONE, GL_ONE );
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
		
	OBJECTNODE * curpos = object_list;
	while (curpos != NULL)
	{
		DrawObject(curpos);
		curpos = curpos -> next;
	}
	
	glPopAttrib();

	
	//reset gl flags
	/*glDisable(GL_BLEND);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_LIGHTING);
	glDisable(GL_ALPHA_TEST);*/
}

void OBJECTS::Add(VERTEX pos, float rotation, string modelname, string texname, bool mip, bool fullbright, bool skybox, bool drv, bool col)
{
	OBJECTNODE * oldfirst = object_list;
	object_list = new OBJECTNODE;
	object_list->next = oldfirst;
	
	object_list->pos.x = pos.x;
	object_list->pos.y = pos.y;
	object_list->pos.z = pos.z;
	
	//if (pos.y == -1337)
	{
//		object_list->pos.y = terrain.GetHeight(object_list->pos.x, object_list->pos.z) + pos.y;
	}
	
	object_list->dir.Rotate(rotation, 0, 1, 0);
	
	OBJECTMODEL * mplist = model_list;
	
	bool found = false;
	int count = 0;
	while (mplist != NULL && !found)
	{
		//cout << mplist->name << "," << modelname << endl;
		if (mplist->name == modelname)
		{
			found = true;
			object_list->model = mplist;
		}
		mplist = mplist -> next;
		count++;
	}
	
	if (!found)
	{
		object_list->model = AddModel(modelname, texname, mip, fullbright, skybox, col);
	}
}

OBJECTNODE::OBJECTNODE()
{
	dir.LoadMultIdent();
}

OBJECTMODEL * OBJECTS::AddModel(string modelname, string texname, bool mip, bool fullbright, bool skybox, bool highlight)
{
	OBJECTMODEL * oldfirst = model_list;
	model_list = new OBJECTMODEL;
	model_list->next = oldfirst;
	model_list->name = modelname;
	model_list->fullbright = fullbright;
	model_list->skybox = skybox;
	
	// prefer jpk over single obj files
	if (loadjpk)
	{
		istream & file = jpk.Pack_fopen(modelname);
		model_list->jmodel.Load(modelname, file);
	}
	else
	{
		model_list->jmodel.Load(path + "/" + modelname);
	}
	
	map <string, GLuint>::iterator tslot = texture_db.find(texname);
	if (tslot == texture_db.end())
	{
		GLuint newtexid = utility.TexLoad(path + "/" + texname, mip);
		texture_db[texname] = newtexid;
		//cout << texname << ": " << newtexid << endl;
		model_list->jmodel.TextureID(newtexid, 0);
	}
	else
	{
		model_list->jmodel.TextureID(tslot->second, 0);
	}
	
	if (highlight)
	{
		string highlight_texname = "gui/highlight.png";
		tslot = texture_db.find(highlight_texname);
		if (tslot == texture_db.end())
		{
			GLuint newtexid = utility.TexLoad(highlight_texname, false);
			texture_db[highlight_texname] = newtexid;
			//cout << texname << ": " << newtexid << endl;
			model_list->jmodel.AdditiveTextureID(newtexid, 1);
		}
		else
		{
			model_list->jmodel.AdditiveTextureID(tslot->second, 1);
		}
	}
	
	//cout << "Addmodel: " << modelname << endl;
	return model_list;
}

void OBJECTS::LoadObjectsFromFolder(string objectpath)
{
	DeleteAll();
	
	ifstream o;
	
	o.open((objectpath+"/list.txt").c_str());
	
	path = objectpath;
	
	if (o)
	{
		// try to load objects.jpk
		loadjpk = jpk.LoadPack(objectpath+"/objects.jpk");
		
		string m;
		string t;
		string extra;
		bool mip;
		bool fb, sb;
		VERTEX p;
		float r;
		bool c, d;
		
		int numparams = utility.iGetParam(o);
		
		if (numparams < 1)
		{
			cerr << "Suspicious numparams value: " << numparams << endl;
			return;
		}
		
		//int count = 0;
		
		while (!o.eof())
		{	
			m = utility.sGetParam(o);
			t = utility.sGetParam(o);
			mip = utility.bGetParam(o);
			fb = utility.bGetParam(o);
			sb = utility.bGetParam(o);
			p.x = utility.fGetParam(o);
			p.y = utility.fGetParam(o);
			p.z = utility.fGetParam(o);
			//r = utility.fGetParam(o);
			p.zero(); //these are actually reserved bits
			r = 0;
			d = utility.bGetParam(o);
			c = utility.bGetParam(o);
			
			for (int i = 0; i < numparams - 10; i++)
			{
				extra = utility.sGetParam(o);
			}
			
			if (m != "" && m != utility.GetEOFString())
				Add(p, r, m, t, mip, fb, sb, d, c);
		}
		
		o.close();
	}
	else
	{
		error_log << "Couldn't open Object List: " << objectpath << "/list.txt" << endl;
	}
}

bool OBJECTS::FindClosestVert(VERTEX orig, VERTEX dir, VERTEX &out)
{
	string obj;
	return FindClosestVert(orig, dir, out, obj);
}

bool OBJECTS::FindClosestVert(VERTEX orig, VERTEX dir, VERTEX &out, string & obj)
{
	OBJECTMODEL * model = NULL;
	if (FindClosestVert(orig, dir, out, obj))
	{
		obj = model->name;
		return true;
	}
	return false;
}

bool OBJECTS::FindClosestVert(VERTEX orig, VERTEX dir, VERTEX &out, OBJECTMODEL * & obj)
{
	float mindist = SELECT_DISTANCE * 2.0;
	VERTEX relobjpos, tvert, rayproj, selvert;
	int i;
	float dotprod = 0;
	
	//dir = dir.normalize().ScaleR(MAX_SELECTION_DIST);
	dir = dir.normalize();
	
	OBJECTNODE * curpos = object_list;
	while (curpos != NULL)
	{
		relobjpos = curpos->pos - orig;
		
		for (i = 0; i < curpos->model->jmodel.GetVerts(0); i++)
		{
			tvert.Set(curpos->model->jmodel.GetVertArray(0)[i].vertex);
			//float tf = tvert.x;
			tvert.z = -tvert.z;
			tvert = relobjpos + tvert;
			
			if (tvert.len() < MAX_SELECTION_DIST)
			{
				dotprod = tvert.dot(dir);
				rayproj = dir.ScaleR(dotprod);
				
				if ((tvert - rayproj).len() < mindist && dotprod > 0)
				{
					mindist = (tvert - rayproj).len();
					selvert = tvert + orig;
					obj = curpos->model;
				}
			}
		}
		
		/*cout << "Num verts: " << curpos->model->jmodel.GetVerts(0) << endl;
		//relobjpos.DebugPrint();
		dir.DebugPrint();
		tvert.DebugPrint();
		cout << tvert.dot(dir) << endl;*/
		
		curpos = curpos -> next;
	}
	
	if (mindist < SELECT_DISTANCE)
	{
		out = selvert;
		return true;
	}
	else
	{
		return false;
	}
}



class OUTPUTRECORD
{
	public:
		VERTEX vert;
		float dotprod;
		float len;
		bool match2;
		
		bool operator< (const OUTPUTRECORD & other) const
		{
			if (match2 != other.match2)
				return other.match2;
			
			return GetMetric() < other.GetMetric();
		}
		
		float GetMetric() const
		{
			if (len < 0.001)
				return 0;
			return dotprod/len;
		}
		
		void DebugPrint()
		{
			std::cout << "(" << vert.x << "," << vert.y << "," << vert.z << "), " << dotprod << ", " << len << ", " << match2 << std::endl;
		}
};

bool OBJECTS::AutoFindClosestVert(VERTEX orig1, VERTEX orig2, VERTEX dir, VERTEX &out)
{
	VERTEX relobjpos, tvert, rayproj;
	int i;
	float dotprod = 0;
	
	//dir = dir.normalize().ScaleR(MAX_SELECTION_DIST);
	dir = dir.normalize();
	
	list <VERTEX> candidatesbetter;
	list <VERTEX> candidates;
	
	list <OUTPUTRECORD> outputs;
	
	OBJECTNODE * curpos = object_list;
	while (curpos != NULL)
	{
		relobjpos = curpos->pos - orig1;
		
		bool inobj1 = false;
		bool inobj2 = false;
		
		//verify that both points are in the object
		for (i = 0; i < curpos->model->jmodel.GetFaces(); i++)
		{
			for (int n = 0; n < 3; n++)
			{
				short vi = curpos->model->jmodel.GetFaceArray(0)[i].vertexIndex[n];
				VERTEX tvert;
				tvert.Set(curpos->model->jmodel.GetVertArray(0)[vi].vertex);
				//tvert.x = -tvert.x;
				tvert.z = -tvert.z;
				//tvert.DebugPrint();
				//orig.DebugPrint();
				if ((tvert - orig1).len() < 0.00001)
				{
					inobj1 = true;
				}
				if ((tvert - orig2).len() < 0.00001)
				{
					inobj2 = true;
				}
			}
		}
		
		if (inobj1 || inobj2)
		{
			//find faces within the object that are attached to the currently selected verts
			for (i = 0; i < curpos->model->jmodel.GetFaces(); i++)
			{
				bool inface1 = false;
				bool inface2 = false;
			
				for (int n = 0; n < 3; n++)
				{
					short vi = curpos->model->jmodel.GetFaceArray(0)[i].vertexIndex[n];
					VERTEX tvert;
					tvert.Set(curpos->model->jmodel.GetVertArray(0)[vi].vertex);
					//tvert.x = -tvert.x;
					tvert.z = -tvert.z;
					//tvert.DebugPrint();
					//orig.DebugPrint();
					if ((tvert - orig1).len() < 0.00001)
					{
						inface1 = true;
					}
					if ((tvert - orig2).len() < 0.00001)
					{
						inface2 = true;
					}
				}
				
				if (inface1 || inface2)
				{
					for (int n = 0; n < 3; n++)
					{
						short vi = curpos->model->jmodel.GetFaceArray(0)[i].vertexIndex[n];
						VERTEX tvert;
						tvert.Set(curpos->model->jmodel.GetVertArray(0)[vi].vertex);
						tvert.z = -tvert.z;
						tvert = tvert + curpos->pos;
						if (inobj1 && inobj2)
							candidatesbetter.push_back(tvert);
						else
							candidates.push_back(tvert);
					}
				}
			}
		}
		
		/*for (i = 0; i < curpos->model->jmodel.GetVerts(0); i++)
		{
			tvert.Set(curpos->model->jmodel.GetVertArray(0)[i].vertex);
			//float tf = tvert.x;
			tvert.z = -tvert.z;
			tvert = relobjpos + tvert;
			
			if (tvert.len() < MAX_AUTO_SELECTION_DIST)
			{
				dotprod = tvert.dot(dir);
				rayproj = dir.ScaleR(dotprod);
				
				if ((tvert - rayproj).len() + (CLOSENESS_AUTO_BIAS*relobjpos.len()) < mindist && dotprod > 0)
				{
					mindist = (tvert - rayproj).len();
					out = tvert + orig;
					
					found = true;
				}
			}
		}*/
		
		/*cout << "Num verts: " << curpos->model->jmodel.GetVerts(0) << endl;
		//relobjpos.DebugPrint();
		dir.DebugPrint();
		tvert.DebugPrint();
		cout << tvert.dot(dir) << endl;*/
		
		curpos = curpos -> next;
	}
	
	//cout << candidates.size() << endl;
	float mindist = MAX_AUTO_SELECTION_DIST * 1000.0;
	for (list <VERTEX>::iterator c = candidatesbetter.begin(); c != candidatesbetter.end(); c++)
	{
		//tvert.Set(*c);
		tvert = *c;
		//float tf = tvert.x;
		//tvert.z = -tvert.z;
		tvert = tvert - orig1;
		
		if (tvert.len() < MAX_AUTO_SELECTION_DIST)
		{
			dotprod = tvert.dot(dir);
			rayproj = dir.ScaleR(dotprod);
			
			if ((tvert - rayproj).len() < mindist && dotprod > 0 && (*c - orig1).len() > 0.001 && (*c - orig2).len() > 0.001)
			{
				outputs.push_back(OUTPUTRECORD());
				outputs.back().match2 = true;
				outputs.back().len = (tvert - rayproj).len();
				VERTEX normtvert = tvert;
				normtvert.normalize();
				outputs.back().dotprod = normtvert.dot(dir);
				outputs.back().vert = tvert + orig1;
				
				//mindist = (tvert - rayproj).len();
				//out = tvert + orig1;
				
				//found = true;
			}
		}
	}
	
	/*if (!outputs.empty())
	{
		std::cout << "Found vert from 2-match candidate:\n";
		out.DebugPrint();
		dir.DebugPrint();
		//tvert.DebugPrint();
		orig1.DebugPrint();
		orig2.DebugPrint();
		(orig2-orig1).DebugPrint();
		//std::cout << "mindist: " << mindist << ", " << sellen << ", " << seldotprod << std::endl;
	}*/
	
	for (list <VERTEX>::iterator c = candidates.begin(); c != candidates.end(); c++)
	{
		//tvert.Set(*c);
		tvert = *c;
		//float tf = tvert.x;
		//tvert.z = -tvert.z;
		tvert = tvert - orig1;
		
		if (tvert.len() < MAX_AUTO_SELECTION_DIST)
		{
			dotprod = tvert.dot(dir);
			rayproj = dir.ScaleR(dotprod);
			
			if ((tvert - rayproj).len() < mindist && dotprod > 0 && (*c - orig1).len() > 0.001 && (*c - orig2).len() > 0.001)
			{
				//mindist = (tvert - rayproj).len();
				//out = tvert + orig1;
				
				//found = true;
				
				outputs.push_back(OUTPUTRECORD());
				outputs.back().match2 = false;
				outputs.back().len = (tvert - rayproj).len();
				VERTEX normtvert = tvert;
				normtvert.normalize();
				outputs.back().dotprod = normtvert.dot(dir);
				outputs.back().vert = tvert + orig1;
			}
		}
	}
	
	outputs.sort();
	
	/*std::cout << "Matches:" << endl;
	for (list <OUTPUTRECORD>::iterator i = outputs.begin(); i != outputs.end(); i++)
		i->DebugPrint();*/
	
	out = outputs.back().vert;
	
	return !outputs.empty();
}
