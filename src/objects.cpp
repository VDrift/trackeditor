#include "objects.h"

OBJECTS::OBJECTS()
{
	error_log.open((settings.GetSettingsDir() + "/logs/objects.log").c_str());
	
	object_list = NULL;
	model_list = NULL;
}

OBJECTS::~OBJECTS()
{
	error_log.close();
	
	//deallocate objects and models
	DeleteAll();
}

void OBJECTS::DeleteAll()
{
	while (object_list != NULL)
		delobject();
	
	while (model_list != NULL)
		delmodel();
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
			if (rd<=-bound)
			{
				return;
			}
		}
	}
	//else cout << "inside object" << endl;
	
	glPushMatrix();
			
	float transform_matrix[16];
	
	object->dir.GetMat(transform_matrix);
	
	glTranslatef(object->pos.x, object->pos.y, object->pos.z);
	glMultMatrixf(transform_matrix);
	glRotated(-90, 1,0,0);
	
	if (object->model != NULL)
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
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
		
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glColor4f(1,1,1,1);
		
		glPopAttrib();
	}
	else
		cout << "NULL model" << endl;
	
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
		
	OBJECTNODE * curpos = object_list;
	while (curpos != NULL)
	{
		DrawObject(curpos);
		curpos = curpos -> next;
	}

	
	//reset gl flags
	/*glDisable(GL_BLEND);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_LIGHTING);
	glDisable(GL_ALPHA_TEST);*/
}

void OBJECTS::Add(VERTEX pos, float rotation, string modelname)
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
		object_list->model = AddModel(modelname);
	}
}

OBJECTNODE::OBJECTNODE()
{
	dir.LoadMultIdent();
}

OBJECTMODEL * OBJECTS::AddModel(string modelname)
{
	OBJECTMODEL * oldfirst = model_list;
	model_list = new OBJECTMODEL;
	model_list->next = oldfirst;
	
	model_list->name = modelname;
	model_list->jmodel.Load(path + "/" + modelname);
	
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
		string m, t;
		VERTEX p;
		float r = 0;
		
		while (!o.eof())
		{
			m = utility.sGetParam(o);
			utility.sGetParam(o); //texture
			utility.bGetParam(o); //mipmap
			utility.bGetParam(o); //fullbright
			utility.bGetParam(o); //skybox
			p.x = utility.fGetParam(o);
			p.y = utility.fGetParam(o);
			p.z = utility.fGetParam(o);
			utility.bGetParam(o); //collide-able
			utility.bGetParam(o); //drive-able
			
			if (m != "" && m != utility.GetEOFString())
				Add(p, r, m);
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

bool OBJECTS::AutoFindClosestVert(VERTEX orig, VERTEX dir, VERTEX &out)
{
	bool found = false;
	VERTEX relobjpos, tvert, rayproj;
	int i;
	float dotprod = 0;
	
	//dir = dir.normalize().ScaleR(MAX_SELECTION_DIST);
	dir = dir.normalize();
	
	list <VERTEX> candidates;
	
	OBJECTNODE * curpos = object_list;
	while (curpos != NULL)
	{
		relobjpos = curpos->pos - orig;
		
		for (i = 0; i < curpos->model->jmodel.GetFaces(); i++)
		{
			bool inface = false;
			for (int n = 0; n < 3; n++)
			{
				short vi = curpos->model->jmodel.GetFaceArray(0)[i].vertexIndex[n];
				VERTEX tvert;
				tvert.Set(curpos->model->jmodel.GetVertArray(0)[vi].vertex);
				//tvert.x = -tvert.x;
				tvert.z = -tvert.z;
				//tvert.DebugPrint();
				//orig.DebugPrint();
				if ((tvert - orig).len() < 0.00001)
				{
					inface = true;
				}
			}
			
			if (inface)
			{
				for (int n = 0; n < 3; n++)
				{
					short vi = curpos->model->jmodel.GetFaceArray(0)[i].vertexIndex[n];
					VERTEX tvert;
					tvert.Set(curpos->model->jmodel.GetVertArray(0)[vi].vertex);
					tvert.z = -tvert.z;
					tvert = tvert + curpos->pos;
					candidates.push_back(tvert);
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
	for (list <VERTEX>::iterator c = candidates.begin(); c != candidates.end(); c++)
	{
		//tvert.Set(*c);
		tvert = *c;
		//float tf = tvert.x;
		//tvert.z = -tvert.z;
		tvert = tvert - orig;
		
		if (tvert.len() < MAX_AUTO_SELECTION_DIST)
		{
			dotprod = tvert.dot(dir);
			rayproj = dir.ScaleR(dotprod);
			
			if ((tvert - rayproj).len() < mindist && dotprod > 0 && (*c - orig).len() > 0.001)
			{
				mindist = (tvert - rayproj).len();
				out = tvert + orig;
				
				found = true;
			}
		}
	}
	
	return found;
}
