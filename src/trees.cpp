#include "trees.h"

TREES::TREES()
{
	error_log.open((settings.GetSettingsDir() + "/logs/trees.log").c_str());
	
	tree_list = NULL;
	
	foliagelist = 0;
	foliagenumx = 0;
	foliagenumy = 0;
	
	comploaded = false;
	
	loaded = false;
}

TREES::~TREES()
{
	error_log.close();
	
	//deallocate tree entities
	DeleteAll();
}

void TREES::DeleteAll()
{
	while (tree_list != NULL)
		deltree();
	
	//if (comploaded)
		//glDeleteTextures(1, &composite_shadow_texture);
}

void TREES::deltree()
{
	if (tree_list == NULL)
		return;
	
	//save old tree
	TREENODE * old = tree_list;
	tree_list = tree_list->next;
	delete old;
}

extern const string TEXTURE_PATH;

void TREES::Load()
{
	if (loaded)
	{
		if (foliagelist != 0)
		{
			glDeleteLists(foliagelist, foliagenumx+foliagenumy);
		}
		
		glDeleteTextures(1, &composite_shadow_texture);
		
		glDeleteTextures(NUM_TREE_TYPES, tree_shadow_tex);
		//glDeleteTextures(2, treetex);
		glDeleteTextures(1, &(treetex[0]));
		glDeleteTextures(1, &(treetex[1]));
		
		composite_shadow_texture = 0;
		foliagelist = 0;
	
		comploaded = false;
		//return;
	}
	
	glGenTextures(1, &composite_shadow_texture);
	
	treetex[0] = utility.TexLoad("trees/canopy.png", true);
	treetex[1] = utility.TexLoad("trees/canopymask1.png", true);
	
	int t, l;
	char treechar[256];
	for (t = 0; t < NUM_TREE_TYPES; t++)
	{
		for (l = 0; l < NUM_TREE_LOD; l++)
		{
			sprintf(treechar, "objects/tree%d_%d.joe", t, l);
			tree_model[t][l].Load(settings.GetFullDataPath(treechar).c_str());
			//cout << treechar << ": " << tree_model[t][l].GetFaces() << endl;
		}
		
		sprintf(treechar, "objects/tree%d_shadow.png", t);
		tree_shadow_tex[t] = utility.TexLoad(settings.GetFullDataPath(treechar).c_str(), true);
	}
	
	loaded = true;
}

void TREES::DrawTree(TREENODE * tree)
{
	//constants
	//float height, width, tileamount;
	//height = 6.0f;
	//width = 4.0f;
	//tileamount = -1.0f;
	
	//const float lod_far = 100*100;
	//const float lod_far = 100*100;
	//const float lod_near = 20*20;
	
	int i;
	
	//float yoffset = 0.0f;
	
	//cout << dx << "," << dy << "," << dz << ":  " << rc << endl;
	
	//if (rc > lod_far*10)
	//	return;

	//use different techniques based on range
	//frustum cull!
	//float spread = 10.0f;
	//if (rc > lod_far)
	
	/*if (rc > lod_far)
	{
		//multi tree billboard
		
		//width *= 6.0f;
		glBindTexture(GL_TEXTURE_2D,treetex[1]);
		tree->dir = cam.dir.ReturnConjugate();
		
		glPushMatrix();
		
		float transform_matrix[16];
		
		tree->dir.GetMat(transform_matrix);
		
		
		glTranslatef(tree->pos.x,tree->pos.y+height/2.0f,tree->pos.z);
		glMultMatrixf(transform_matrix);
		
		glBegin(GL_QUADS);
		glColor4f(1.0f,1.0f,1.0f,1.0f);
			glTexCoord2f(0,0);glVertex3f(width/2.0f,-height/2.0f-yoffset,0);
			glTexCoord2f(0,tileamount);glVertex3f(width/2.0f,height/2.0f-yoffset,0);
			glTexCoord2f(tileamount,tileamount);glVertex3f(-width/2.0f,height/2.0f-yoffset,0);
			glTexCoord2f(tileamount,0);glVertex3f(-width/2.0f,-height/2.0f-yoffset,0);
		glEnd();
		
		glPopMatrix();
	}*/
	/*else if (rc > lod_near && rc <= lod_far)
	{
		//single tree billboard
		glBindTexture(GL_TEXTURE_2D,treetex[0]);

		for (i = 0; i < TREES_PER_NODE; i++)
		{
			tree->tree_ent[i].dir = cam.dir.ReturnConjugate();
			
			glPushMatrix();
			
			float transform_matrix[16];
			
			tree->tree_ent[i].dir.GetMat(transform_matrix);
			
			glTranslatef(tree->tree_ent[i].x,tree->tree_ent[i].y+height/2.0f,tree->tree_ent[i].z);
			glMultMatrixf(transform_matrix);	
			
			glBegin(GL_QUADS);
			glColor4f(1.0f,1.0f,1.0f,1.0f);
			
			glTexCoord2f(0,0);glVertex3f(width/2.0f,-height/2.0f-yoffset,0);
			glTexCoord2f(0,tileamount);glVertex3f(width/2.0f,height/2.0f-yoffset,0);
			glTexCoord2f(tileamount,tileamount);glVertex3f(-width/2.0f,height/2.0f-yoffset,0);
			glTexCoord2f(tileamount,0);glVertex3f(-width/2.0f,-height/2.0f-yoffset,0);
			
			glEnd();
			
			glPopMatrix();
		}
	}*/
	//else
	{
		//single tree fixed X sprite
		//glBindTexture(GL_TEXTURE_2D,treetex[0]);
		
		//for very far away tree groups, don't LOD, just fade 'em out
		float dx, dy, dz, rc;
		dx=tree->pos.x+cam.position.x; dy=tree->pos.y+cam.position.y; dz=tree->pos.z+cam.position.z;
		dy = 0;
    		rc=dx*dx+dy*dy+dz*dz;
		
		//float fadeat = 250;
		//float fadeto = 300;
		float fadeat, fadeto;
		fadeat = 500;
		fadeto = fadeat + 50;
		if (state.GetTreeDetail() == "High")
		{
			fadeat = 800;
			fadeto = fadeat + 100;
		}
		if (state.GetTreeDetail() == "Max")
		{
			fadeat = 1500;
			fadeto = fadeat + 250;
		}
		
		fadeat = fadeat * fadeat;
		fadeto = fadeto * fadeto;
		float fade = (rc - fadeat)/(fadeto - fadeat);
		if (fade > 1)
			return;
		/*if (fade > 1)
			fade = 1;*/
		if (fade < 0)
			fade = 0;
		fade = 1 - fade;
		fade = fade;
		float brt = 0.75;
		glColor4f(brt,brt,brt,fade);
		glAlphaFunc(GL_GREATER, fade*0.9f);
		//glColor4f(brt,brt,brt,1.0);
		//glAlphaFunc(GL_GREATER, 0.9f);
		
		
		int lod = 2;
		
		float lodtran1, lodtran2;
		lodtran1 = 150;
		lodtran2 = 350;
		
		if (state.GetTreeDetail() == "High" || state.GetTreeDetail() == "Max")
		{
			lodtran1 = 300;
			lodtran2 = 500;
		}
		
		lodtran1 *= lodtran1;
		lodtran2 *= lodtran2;
		
		if (rc > lodtran2)
			lod = 0;
		else if (rc > lodtran1)
			lod = 1;
		
		if (state.GetTreeDetail() == "Low")
			lod = 0;
		
		for (i = 0; i < tree->numtrees; i++)
		{
			//range cull
			/*dx=tree->tree_ent[i].x+cam.position.x; dy=tree->tree_ent[i].y+cam.position.y; dz=tree->tree_ent[i].z+cam.position.z;
    			rc=dx*dx+dy*dy+dz*dz;*/
			float bound = tree_model[tree->type][lod].GetRadius()*2.0f;
			
			int f;
			for (f=0;f<6;f++) 
			{
				float rd=cam.frustum[f][0]*tree->tree_ent[i].x+
				cam.frustum[f][1]*tree->tree_ent[i].y+
				cam.frustum[f][2]*tree->tree_ent[i].z+
				cam.frustum[f][3];
				if (rd<=-bound)
				{
					//cout << rd << "," << -bound << endl;
					return;
				}
			}
		
			//tree->tree_ent[i].dir = cam.dir.ReturnConjugate();
			//tree->tree_ent[i].dir.LoadMultIdent();
			
			glPushMatrix();
			
			float transform_matrix[16];
			
			tree->tree_ent[i].dir.GetMat(transform_matrix);
			
			glTranslatef(tree->tree_ent[i].x,tree->tree_ent[i].y,tree->tree_ent[i].z);
			glMultMatrixf(transform_matrix);
			glRotated(-90, 1,0,0);
			
			glScaled(tree->scale[i]+0.75,tree->scale[i]+0.75,tree->scale[i]+0.75);
			
			/*glBegin(GL_QUADS);
			glColor4f(1.0f,1.0f,1.0f,1.0f);
			
				glTexCoord2f(0,0);glVertex3f(width/2.0f,-height/2.0f-yoffset,0);
				glTexCoord2f(0,tileamount);glVertex3f(width/2.0f,height/2.0f-yoffset,0);
				glTexCoord2f(tileamount,tileamount);glVertex3f(-width/2.0f,height/2.0f-yoffset,0);
				glTexCoord2f(tileamount,0);glVertex3f(-width/2.0f,-height/2.0f-yoffset,0);
			
				glTexCoord2f(0,0);glVertex3f(0,-height/2.0f-yoffset,width/2.0f);
				glTexCoord2f(0,tileamount);glVertex3f(0,height/2.0f-yoffset,width/2.0f);
				glTexCoord2f(tileamount,tileamount);glVertex3f(0,height/2.0f-yoffset,-width/2.0f);
				glTexCoord2f(tileamount,0);glVertex3f(0,-height/2.0f-yoffset,-width/2.0f);
			glEnd();*/
			tree_model[tree->type][lod].DrawStatic();
			
			glPopMatrix();
		}
	}
}

void TREES::Draw()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	//setup gl flags
	glEnable(GL_BLEND);
	//glDisable( GL_TEXTURE_2D );
	glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D,treetex[0]);
	glAlphaFunc(GL_GREATER, 0.9f);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	//glBlendFunc( GL_ONE, GL_ONE );

	glEnable(GL_LIGHTING);
	glAlphaFunc(GL_GREATER, 0.9f);
	glColor4f(0.75f,0.75f,0.75f,1.0f);
	//glDisable(GL_BLEND);
	//glDisable(GL_ALPHA_TEST);
	
	if (state.GetTreeDetail() != "Off")
		DrawFoliage();
	
	glDisable( GL_LIGHTING);
	glPushMatrix();	
	TREENODE * curpos = tree_list;
	while (curpos != NULL)
	{
		if (state.GetTreeDetail() != "Off" && state.GetTreeDetail() != "FoliageOnly")
			DrawTree(curpos);
		curpos = curpos -> next;
		
		//cout << "Tree draw" << endl;
	}
	glPopMatrix();
	
	//glAlphaFunc(GL_ALWAYS, 1.0);
	
	//reset gl flags
	glDisable(GL_BLEND);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_LIGHTING);
	glDisable(GL_ALPHA_TEST);
	glPopAttrib();
}

void TREES::Add(VERTEX pos, float spread, int type, int number)
{
	if (number <= 0)
		return;

	TREENODE * oldfirst = tree_list;
	tree_list = new TREENODE;
	tree_list->next = oldfirst;
	
	tree_list->pos.x = pos.x;
	tree_list->pos.y = pos.y;
	tree_list->pos.z = pos.z;
	tree_list->type = type;
	
	float xoffset, zoffset;
	xoffset = zoffset = 0;
	
	if (number > TREES_PER_NODE)
		number = TREES_PER_NODE;
		
	tree_list->numtrees = number;
	
	int i;
	for (i = 0; i < number; i++)
	{
		//xoffset = (float) (rand()%(unsigned int)(spread*10.0f)) / 10.0f;
		//zoffset = (float) (rand()%(unsigned int)(spread*10.0f)) / 10.0f;
		float rot = ((float)rand()/RAND_MAX)*3.141592;
		tree_list->tree_ent[i].dir.Rotate(rot, 0, 1, 0);

		//float yoffset = -((float) rand()/RAND_MAX)*2.0;
		float yoffset = 0.0;

		xoffset = ((float) rand()/RAND_MAX)*spread;
		zoffset = ((float) rand()/RAND_MAX)*spread;
		tree_list->tree_ent[i].x = pos.x+xoffset;
		tree_list->tree_ent[i].z = pos.z+zoffset;
		
		tree_list->tree_ent[i].y = terrain.GetHeight(tree_list->tree_ent[i].x,tree_list->tree_ent[i].z)+yoffset;
		
//		float treeheight = tree_list->tree_ent[i].y;
		float v[3];
		v[0] = tree_list->tree_ent[i].x;
		v[1] = tree_list->tree_ent[i].y;
		v[2] = tree_list->tree_ent[i].z;
		tree_list->tree_ent[i].x = v[0];
		tree_list->tree_ent[i].y = v[1];
		tree_list->tree_ent[i].z = v[2];
		
		tree_list->scale[i] = (float)rand()/RAND_MAX;
		
		float shadowsize = 30.0;
		yoffset = 0.0;
		
		tree_list->shadowpos[i][0] = tree_list->tree_ent[i].pos();
		tree_list->shadowpos[i][1] = tree_list->tree_ent[i].pos();
		tree_list->shadowpos[i][2] = tree_list->tree_ent[i].pos();
		tree_list->shadowpos[i][3] = tree_list->tree_ent[i].pos();
		
		tree_list->shadowpos[i][0].x -= shadowsize; tree_list->shadowpos[i][0].z -= shadowsize;
		tree_list->shadowpos[i][1].x += shadowsize; tree_list->shadowpos[i][1].z -= shadowsize;
		tree_list->shadowpos[i][2].x += shadowsize; tree_list->shadowpos[i][2].z += shadowsize;
		tree_list->shadowpos[i][3].x -= shadowsize; tree_list->shadowpos[i][3].z += shadowsize;

		int d;
		for (d = 0; d < 4; d++)
		{
			tree_list->shadowpos[i][d].y = terrain.GetHeight(tree_list->shadowpos[i][d].x,tree_list->shadowpos[i][d].z)+yoffset;
			tree_list->shadowpos[i][d].y = tree_list->shadowpos[i][d].y - tree_list->tree_ent[i].y;
			tree_list->shadowpos[i][d].x = tree_list->shadowpos[i][d].x - tree_list->tree_ent[i].x;
			tree_list->shadowpos[i][d].z = tree_list->shadowpos[i][d].z - tree_list->tree_ent[i].z;
			//tree_list->shadowpos[i][d] = tree_list->shadowpos[i][d] - tree_list
			
			//tree_list->shadowpos[i][d].DebugPrint();
		}
		//cout << endl;
	}
}

TREEENTITY::TREEENTITY()
{
	x = y = z = 0;
	dir.LoadMultIdent();
}

void TREES::ReadFromFile(string treemapfile, string foliagemapfile)
{
	DeleteAll();
	
	srand(1337);
	
	SDL_Surface * tree_map;
	
	if ((tree_map=IMG_Load(treemapfile.c_str())))
	{
		if (tree_map->format->BytesPerPixel != 1)
		{
			error_log << "Tree map does not have the correct number of channels: " << treemapfile << "\n";
		}
		else
		{
			int w = tree_map->w;
			int h = tree_map->h;
			float t_xoff = terrain.GetOffset().x;
			float t_zoff = terrain.GetOffset().z;
			float t_w = terrain.GetSize().x;
			float t_h = terrain.GetSize().z;
			
			int x,y;
			for (y = 0; y < h; y++)
			{
				for (x = 0; x < w; x++)
				{
					float density = (float)(((unsigned char *) tree_map->pixels)[y*w+x])/255.0;
					if (density < 0)
						density = 0;
					if (density > 1)
						density = 1;
					
					VERTEX pixelpos;
					pixelpos.x = ((float) x / (float) w) * t_w + t_xoff;
					pixelpos.z = ((float) y / (float) h) * t_h + t_zoff;
					
					float pixelwidth = (1.0 / (float) w) * t_w;
					float pixelheight = (1.0 / (float) h) * t_h;
					
					//pixelpos.x += pixelwidth/2.0;
					//pixelpos.z += pixelheight/2.0;
					
					pixelpos.x -= pixelwidth/2.0;
					pixelpos.z -= pixelheight/2.0;
					
					float pixelspread = pixelwidth;
					if (pixelheight > pixelspread)
						pixelspread = pixelheight;
					
					/*float prob = density;
					if (prob > (float) rand() / RAND_MAX)
						num = 1;
					else
						num = 0;*/
						
					//int num = (int)(density * TREES_PER_NODE);
					int i;
					for (i = 0; i < NUM_TREE_TYPES; i++)
					{
						float prob = (float) rand() / RAND_MAX;
						//prob = prob * prob * prob * prob * prob;
						//prob = pow(prob, 10.0);
						//prob = prob * prob * prob * prob;
						prob = pow((double) prob, 1.5);
						int num = (int)(prob * density * TREES_PER_NODE);
						Add(pixelpos, pixelspread, i, num);
					}
					
					//Add(pixelpos, pixelspread, 0, 1);
				}
			}
		}
		
		//(float)(((unsigned char *) htemp->pixels)[i])/255.0;
		SDL_FreeSurface(tree_map);
	}
	else
	{
		//quit, bitmap not found
		error_log << "Could not find tree map: " << treemapfile << "\n";
	}
	
	if ((tree_map=IMG_Load(foliagemapfile.c_str())))
	{
		if (tree_map->format->BytesPerPixel != 1)
		{
			error_log << "Tree map does not have the correct number of channels: " << treemapfile << "\n";
		}
		else
		{
			int w = tree_map->w;
			int h = tree_map->h;
			float t_xoff = terrain.GetOffset().x;
			float t_zoff = terrain.GetOffset().z;
			float t_w = terrain.GetSize().x;
			float t_h = terrain.GetSize().z;
			
			int x, y;
			
			//h = 25;
			//w = 25;
			//h = 50;
			//w = 50;
			/*foliagenumx = 0;
			//count the number of foliage patches
			for (y = 0; y < h; y++)
			{
				float prevd = 0;
				int l = 0;
				
				for (x = 0; x < w; x++)
				{
					float fx = (float) x / w;
					float fy = (float) y / h;
					float d = utility.GetValue(tree_map, 0, fx, fy, false);
					l++;
					if (d == 0 && prevd != 0) //only count if it's the end of a patch
					{
						//cout << l << endl;
						l = 0;
						foliagenumx++;
					}
					prevd = d;
				}
				
				//the end of a row is always the end of a patch
				if (prevd != 0)
					foliagenumx++;
			}
			
			foliagenumy = 0;
			for (x = 0; x < w; x++)
			{
				float prevd = 0;
				int l = 0;
				
				for (y = 0; y < h; y++)
				{
					float fx = (float) x / w;
					float fy = (float) y / h;
					float d = utility.GetValue(tree_map, 0, fx, fy, false);
					l++;
					if (d == 0 && prevd != 0)
					{
						//cout << l << endl;
						l = 0;
						foliagenumy++;
					}
					prevd = d;
				}
				
				if (prevd != 0)
					foliagenumy++;
			}*/
			
			//cout << foliagenumx << "," << foliagenumy << endl;
			
			w = 75;
			h = 75;
			
			int n = 0;
			foliagenumx = foliagenumy = 0;
			//foliagenum = 0;
			{
				//start a patch
				bool open = false;
				open = true;
				float xoffset = 0;
				float yoffset = 0;
				for (y = 0; y < h; y++)
				{
					float prevd = 0;
					int l = 0;
					float oldx = 0;
//					float tilex = 4;
//					float tiley = 4;
					//yoffset = y/tiley;
					yoffset = (float)rand()/RAND_MAX;
					xoffset = (float)rand()/RAND_MAX;
					
					for (x = 0; x < w; x++)
					{
						//start a row
						float fx = (float) x / w;
						float fy = (float) y / h;
						float d = utility.GetValue(tree_map, 0, fx, fy, false);
						
						float mheight, height;
						
						if (d != 0 || prevd != 0)
						{
							mheight = 50.0f;
							height = 0;
						}
						
						l++;
						
						if (d == 0 && prevd != 0) //it's the end of a patch
						{
							if (!open)
							{
								//error_log << "Foliage error:  close without open" << endl;
							}
							else
							{
								//glEnd();
								//glEndList();
							}
							
							open = false;
							
							l = 0;
							//foliagenum++;
							n++;
							
							//only start a new patch if we've got some left to do
							//if (n < foliagenumx)
							{
								//glNewList(foliagelist+n, GL_COMPILE);
								//glBegin(GL_TRIANGLE_STRIP);
								if (open)
									error_log << "Foliage error:  open without close" << endl;
								open = true;
							}
						}
						
						prevd = d;
						oldx = fx;
					}
					
					if (open)
					{
						//glEnd();
						//glEndList();
						
						if (!open)
							error_log << "Foliage error:  close without open" << endl;
						open = false;
						
						l = 0;
						//foliagenum++;
						n++;
						
						//if (n < foliagenumx)
						{
							//glNewList(foliagelist+n, GL_COMPILE);
							//glBegin(GL_TRIANGLE_STRIP);
							if (open)
								error_log << "Foliage error:  open without close" << endl;
							open = true;
						}
					}
				}
				//cout << "Foliage points: " << foliagenum << endl;
				if (open)
				{
					//error_log << "Foliage error:  no close" << endl;
					//glEnd();
					//glEndList();
				}
			} //done counting x-coord foliage
			
			foliagenumx = n;
			
			//cout << n << ",";
			
			{ //count y-coord foliage
				bool open = false;
				n = 0;
				open = true;
				float xoffset = 0;
				float yoffset = 0;
				for (x = 0; x < w; x++)
				{
					float prevd = 0;
					int l = 0;
					float oldy = 0;
//					float tilex = 4;
//					float tiley = 4;
					//yoffset = y/tiley;
					yoffset = (float)rand()/RAND_MAX;
					xoffset = (float)rand()/RAND_MAX;
					
					for (y = 0; y < h; y++)
					{
						float fx = (float) x / w;
						float fy = (float) y / h;
						float d = utility.GetValue(tree_map, 0, fx, fy, false);
						
						float height, mheight;
						
						if (d != 0 || prevd != 0)
						{
							mheight = 50.0f;
							height = 0;
						}
						
						l++;
						
						if (d == 0 && prevd != 0)
						{
							if (!open)
							{
								//error_log << "Foliage error:  close without open" << endl;
							}
							else
							{
								//glEnd();
								//glEndList();
							}
							
							open = false;
							
							l = 0;
							//foliagenum++;
							n++;
							
							//if (n < foliagenumy)
							{
								//glNewList(foliagelist+foliagenumx+n, GL_COMPILE);
								//glBegin(GL_TRIANGLE_STRIP);
								if (open)
									error_log << "Foliage error:  open without close" << endl;
								open = true;
							}
						}
						
						prevd = d;
						oldy = fy;
					}
					
					if (open)
					{
						//glEnd();
						//glEndList();
						
						if (!open)
							error_log << "Foliage error:  close without open" << endl;
						open = false;
						
						l = 0;
						//foliagenum++;
						n++;
						
						//if (n < foliagenumy)
						{
							//glNewList(foliagelist+foliagenumx+n, GL_COMPILE);
							//glBegin(GL_TRIANGLE_STRIP);
							if (open)
								error_log << "Foliage error:  open without close" << endl;
							open = true;
						}
					}
				}
				//cout << "Foliage points: " << foliagenum << endl;
				if (open)
				{
					//error_log << "Foliage error:  no close" << endl;
					//glEnd();
					//glEndList();
				}
			} //done counting y-coord foliage
			
			foliagenumy = n;
			
			//cout << foliagenumx << "," << foliagenumy << endl;
			
			n = 0;
			
			float textile = 2;
			
			//foliagenum = 0;
			{
				//start a patch
				bool open = false;
				if (foliagenumx+foliagenumy > 0)
				{
					foliagelist = glGenLists(foliagenumx+foliagenumy);
					glNewList (foliagelist, GL_COMPILE);
					glBegin(GL_TRIANGLE_STRIP);
					open = true;
				}
				float xoffset = 0;
				float yoffset = 0;
				for (y = 0; y < h; y++)
				{
					float prevd = 0;
					int l = 0;
					float oldx = 0;
					float tilex = textile;
					float tiley = textile;
					//yoffset = y/tiley;
					yoffset = (float)rand()/RAND_MAX;
					xoffset = (float)rand()/RAND_MAX;
					
					for (x = 0; x < w; x++)
					{
						//start a row
						float fx = (float) x / w;
						float fy = (float) y / h;
						float d = utility.GetValue(tree_map, 0, fx, fy, false);
						
						if (d != 0 || prevd != 0)
						{
							float mheight = 50.0f;
							float height = 0;
							
							float randomwidth = t_w / w;
							
							//special case, first point
							if (d != 0 && prevd == 0)
							{
								height = -mheight + terrain.GetHeight(oldx*t_w+t_xoff,fy*t_h+t_zoff);
								glNormal3f(0,1,0);
								utility.TexCoord2d2f(0, 0+xoffset, -1.0/tiley+yoffset);
								//utility.TexCoord2d2f(0, 0+xoffset, -mheight/tiley+yoffset);
								utility.TexCoord2d2f(1, 0+xoffset, -1);
								glVertex3f(oldx*t_w+t_xoff, height, fy*t_h+t_zoff+((float)rand()/RAND_MAX)*randomwidth);
							}
							
							//special case, last point
							if (d == 0 && prevd != 0)
							{
								height = -mheight + terrain.GetHeight(fx*t_w+t_xoff,fy*t_h+t_zoff);
								glNormal3f(0,1,0);
								//utility.TexCoord2d2f(0, (float)l/tilex+xoffset, -1.0/tiley+yoffset);
								utility.TexCoord2d2f(0, (float)l/tilex+xoffset, -1.0/tiley+yoffset);
								utility.TexCoord2d2f(1, (float)l/tilex+xoffset, -1);
								glVertex3f(fx*t_w+t_xoff, height, fy*t_h+t_zoff+((float)rand()/RAND_MAX)*randomwidth);
							}
							else if (d != 0)
							{
								//normal case, draw two points in the strip
								float dm = (d*0.5)+0.5;
								
								height = mheight * (dm) + terrain.GetHeight(fx*t_w+t_xoff,fy*t_h+t_zoff);
								//height = mheight + terrain.GetHeight(fx*t_w+t_xoff,fy*t_h+t_zoff);
								glNormal3f(0,1,0);
								//utility.TexCoord2d2f(0, (float)l/tilex+xoffset, 1.0/(tiley*d)+yoffset);
								utility.TexCoord2d2f(0, (float)l/tilex+xoffset, d/(tiley)+yoffset);
								utility.TexCoord2d2f(1, (float)l/tilex+xoffset, 1);
								//glTexCoord2f((float)l/2.0, mheight);
								glVertex3f(fx*t_w+t_xoff, height, fy*t_h+t_zoff+((float)rand()/RAND_MAX)*randomwidth);
								
								//cout << height << endl;
								
								height = -mheight + terrain.GetHeight(fx*t_w+t_xoff,fy*t_h+t_zoff);
								glNormal3f(0,1,0);
								//utility.TexCoord2d2f(0, (float)l/tilex+xoffset, -1.0/(tiley)+yoffset);
								utility.TexCoord2d2f(0, (float)l/tilex+xoffset, -1.0/tiley+yoffset);
								utility.TexCoord2d2f(1, (float)l/tilex+xoffset, -1);
								glVertex3f(fx*t_w+t_xoff, height, fy*t_h+t_zoff+((float)rand()/RAND_MAX)*randomwidth);
							}
						}
						
						l++;
						
						if (d == 0 && prevd != 0) //it's the end of a patch
						{
							if (!open)
							{
								//error_log << "Foliage error:  close without open" << endl;
							}
							else
							{
								glEnd();
								glEndList();
							}
							
							open = false;
							
							l = 0;
							//foliagenum++;
							n++;
							
							//only start a new patch if we've got some left to do
							if (n < foliagenumx)
							{
								glNewList(foliagelist+n, GL_COMPILE);
								glBegin(GL_TRIANGLE_STRIP);
								if (open)
									error_log << "Foliage error:  open without close" << endl;
								open = true;
							}
						}
						
						prevd = d;
						oldx = fx;
					}
					
					if (open)
					{
						glEnd();
						glEndList();
						
						if (!open)
							error_log << "Foliage error:  close without open" << endl;
						open = false;
						
						l = 0;
						//foliagenum++;
						n++;
						
						if (n < foliagenumx)
						{
							glNewList(foliagelist+n, GL_COMPILE);
							glBegin(GL_TRIANGLE_STRIP);
							if (open)
								error_log << "Foliage error:  open without close" << endl;
							open = true;
						}
					}
				}
				//cout << "Foliage points: " << foliagenum << endl;
				if (open)
				{
					//error_log << "Foliage error:  no close" << endl;
					glEnd();
					glEndList();
				}
			} //done drawing x-coord foliage
			
			//cout << n << ",";
			
			{ //draw y-coord foliage
				bool open = false;
				n = 0;
				if (foliagenumx+foliagenumy > 0)
				{
					//foliagelist = glGenLists(foliagenumx+foliagenumy);
					//n++;
					glNewList (foliagelist+foliagenumx, GL_COMPILE);
					glBegin(GL_TRIANGLE_STRIP);
					open = true;
				}
				float xoffset = 0;
				float yoffset = 0;
				for (x = 0; x < w; x++)
				{
					float prevd = 0;
					int l = 0;
					float oldy = 0;
					float tilex = textile;
					float tiley = textile;
					//yoffset = y/tiley;
					yoffset = (float)rand()/RAND_MAX;
					xoffset = (float)rand()/RAND_MAX;
					
					for (y = 0; y < h; y++)
					{
						float fx = (float) x / w;
						float fy = (float) y / h;
						float d = utility.GetValue(tree_map, 0, fx, fy, false);
						
						if (d != 0 || prevd != 0)
						{
							float mheight = 50.0f;
							float height = 0;
							
							float randomwidth = t_h / h;
							
							//special case, first point
							if (d != 0 && prevd == 0)
							{
								height = -mheight + terrain.GetHeight(fx*t_w+t_xoff,oldy*t_h+t_zoff);
								glNormal3f(0,1,0);
								utility.TexCoord2d2f(0, 0+xoffset, -1.0/tiley+yoffset);
								utility.TexCoord2d2f(1, 0+xoffset, -1);
								glVertex3f(fx*t_w+t_xoff+((float)rand()/RAND_MAX)*randomwidth, height, oldy*t_h+t_zoff);
							}
							
							//special case, last point
							if (d == 0 && prevd != 0)
							{
								height = -mheight + terrain.GetHeight(fx*t_w+t_xoff,fy*t_h+t_zoff);
								glNormal3f(0,1,0);
								utility.TexCoord2d2f(0, (float)l/tilex+xoffset, -1.0/tiley+yoffset);
								utility.TexCoord2d2f(1, (float)l/tilex+xoffset, -1);
								glVertex3f(fx*t_w+t_xoff+((float)rand()/RAND_MAX)*randomwidth, height, fy*t_h+t_zoff);
							}
							else if (d != 0)
							{
								float dm = (d*0.5)+0.5;
								
								height = mheight * (dm) + terrain.GetHeight(fx*t_w+t_xoff,fy*t_h+t_zoff);
								//height = mheight + terrain.GetHeight(fx*t_w+t_xoff,fy*t_h+t_zoff);
								glNormal3f(0,1,0);
								//utility.TexCoord2d2f(0, (float)l/tilex+xoffset, 1.0/(tiley*d)+yoffset);
								utility.TexCoord2d2f(0, (float)l/tilex+xoffset, d/(tiley)+yoffset);
								utility.TexCoord2d2f(1, (float)l/tilex+xoffset, 1);
								//glTexCoord2f((float)l/2.0, mheight);
								glVertex3f(fx*t_w+t_xoff+((float)rand()/RAND_MAX)*randomwidth, height, fy*t_h+t_zoff);
								
								//cout << height << endl;
								
								height = -mheight + terrain.GetHeight(fx*t_w+t_xoff,fy*t_h+t_zoff);
								glNormal3f(0,1,0);
								utility.TexCoord2d2f(0, (float)l/tilex+xoffset, -1.0/(tiley)+yoffset);
								utility.TexCoord2d2f(1, (float)l/tilex+xoffset, -1);
								glVertex3f(fx*t_w+t_xoff+((float)rand()/RAND_MAX)*randomwidth, height, fy*t_h+t_zoff);
							}
						}
						
						l++;
						
						if (d == 0 && prevd != 0)
						{
							if (!open)
							{
								//error_log << "Foliage error:  close without open" << endl;
							}
							else
							{
								glEnd();
								glEndList();
							}
							
							open = false;
							
							l = 0;
							//foliagenum++;
							n++;
							
							if (n < foliagenumy)
							{
								glNewList(foliagelist+foliagenumx+n, GL_COMPILE);
								glBegin(GL_TRIANGLE_STRIP);
								if (open)
									error_log << "Foliage error:  open without close" << endl;
								open = true;
							}
						}
						
						prevd = d;
						oldy = fy;
					}
					
					if (open)
					{
						glEnd();
						glEndList();
						
						if (!open)
							error_log << "Foliage error:  close without open" << endl;
						open = false;
						
						l = 0;
						//foliagenum++;
						n++;
						
						if (n < foliagenumy)
						{
							glNewList(foliagelist+foliagenumx+n, GL_COMPILE);
							glBegin(GL_TRIANGLE_STRIP);
							if (open)
								error_log << "Foliage error:  open without close" << endl;
							open = true;
						}
					}
				}
				//cout << "Foliage points: " << foliagenum << endl;
				if (open)
				{
					//error_log << "Foliage error:  no close" << endl;
					glEnd();
					glEndList();
				}
			} //done drawing y-coord foliage
			
			//cout << n << endl;
		}
		
		SDL_FreeSurface(tree_map);
		
		BuildCompositeShadow();
	}
	else
	{
		//quit, bitmap not found
		error_log << "Could not find tree map: " << treemapfile << "\n";
	}
}

void TREES::DrawFoliage()
{
	glBindTexture(GL_TEXTURE_2D,treetex[0]);
	
	utility.SelectTU(1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, treetex[1]);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_RGB_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB_ARB,GL_SRC_COLOR);
	//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
	//glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB_ARB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_ARB, GL_REPLACE);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ALPHA_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA_ARB,GL_SRC_ALPHA);
	//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, GL_TEXTURE);
	//glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_ALPHA_ARB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA_ARB, GL_REPLACE);
	
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 1);
	
	utility.SelectTU(0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	int i;
	for (i = 0; i < foliagenumx + foliagenumy; i++)
	{
		glCallList(foliagelist+i);
	}
	
	utility.SelectTU(1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glDisable(GL_TEXTURE_2D);
	utility.SelectTU(0);
}

void TREES::DrawShadows()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(0);
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1,1,1,1);
	
	//glPolygonOffset(-10.0,10.0);
	//glPolygonOffset(-40.0,0.0);
	//glPolygonOffset(100.0,100.0);
	//glPolygonOffset(-1000.0, -1000.0);
	//glPolygonOffset(-11.0, -40.0);
	//glPolygonOffset(-1.1, -400.0);
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glDepthFunc(GL_ALWAYS);
	
	TREENODE * curpos = tree_list;
	while (curpos != NULL)
	{
		//DrawTree(curpos);
		DrawTreeShadow(curpos);
		
		curpos = curpos -> next;
	}
	
	glDepthMask(1);
	glDisable(GL_POLYGON_OFFSET_FILL);
	glPopAttrib();
}

void TREES::DrawTreeShadow(TREENODE * tree)
{
	return;
	
	int i;
	
	{
		//for very far away tree groups, don't draw any shadows
		float dx, dy, dz, rc;
		dx=tree->pos.x+cam.position.x; dy=tree->pos.y+cam.position.y; dz=tree->pos.z+cam.position.z;
    		rc=dx*dx+dy*dy+dz*dz;
		
		//float fadeat = 250;
		//float fadeto = 300;
		float fadeat = 500;
		float fadeto = fadeat + 50;
		
		fadeat = fadeat * fadeat;
		fadeto = fadeto * fadeto;
		float fade = (rc - fadeat)/(fadeto - fadeat);
		if (fade > 1)
			return;
		if (fade < 0)
			fade = 0;
		fade = 1 - fade;
		fade = fade;
//		float brt = 0.75;
		glColor4f(1,1,1,fade);
		//glAlphaFunc(GL_GREATER, fade*0.9f);
		//glColor4f(brt,brt,brt,1.0);
		//glAlphaFunc(GL_GREATER, 0.9f);
		
		for (i = 0; i < tree->numtrees; i++)
		{
			//shadow texture
			glBindTexture(GL_TEXTURE_2D,tree_shadow_tex[tree->type]);
			
			//range cull
			/*dx=tree->tree_ent[i].x+cam.position.x; dy=tree->tree_ent[i].y+cam.position.y; dz=tree->tree_ent[i].z+cam.position.z;
    			rc=dx*dx+dy*dy+dz*dz;*/
			/*int f;
			for (f=0;f<6;f++) 
			{
				float rd=cam.frustum[f][0]*tree->tree_ent[i].x+
				cam.frustum[f][1]*tree->tree_ent[i].y+
				cam.frustum[f][2]*tree->tree_ent[i].z+
				cam.frustum[f][3];
				if (rd<=-bound)
				{
					//cout << rd << "," << -bound << endl;
					return;
				}
			}*/
		
			//tree->tree_ent[i].dir = cam.dir.ReturnConjugate();
			//tree->tree_ent[i].dir.LoadMultIdent();
			
			glPushMatrix();
			
			float transform_matrix[16];
			
			tree->tree_ent[i].dir.GetMat(transform_matrix);
			
			glTranslatef(tree->tree_ent[i].x,tree->tree_ent[i].y,tree->tree_ent[i].z);
			//glMultMatrixf(transform_matrix);
			//glRotated(-90, 1,0,0);
			
			//glScaled(tree->scale[i]+0.75,tree->scale[i]+0.75,tree->scale[i]+0.75);
						
//			float bound = tree_model[tree->type][2].GetRadius();
			
			/*glBegin(GL_QUADS);
				glTexCoord2d(0,0);
				glVertex3fv(tree->shadowpos[i][0].v3());
				//glVertex3fv(-bound,yoffset,-bound);
				glTexCoord2d(1,0);
				glVertex3fv(tree->shadowpos[i][1].v3());
				//glVertex3f(bound,yoffset,-bound);
				glTexCoord2d(1,1);
				glVertex3fv(tree->shadowpos[i][2].v3());
				//glVertex3f(bound,yoffset,bound);
				glTexCoord2d(0,1);
				glVertex3fv(tree->shadowpos[i][3].v3());
				//glVertex3f(-bound,yoffset,bound);
			glEnd();*/
			
			glBegin(GL_TRIANGLE_FAN);
				glTexCoord2d(0.5,0.5);
				glVertex3f(0,0.0,0);
				
				glTexCoord2d(0,0);
				glVertex3fv(tree->shadowpos[i][0].v3());
				
				glTexCoord2d(1,0);
				glVertex3fv(tree->shadowpos[i][1].v3());
				
				glTexCoord2d(1,1);
				glVertex3fv(tree->shadowpos[i][2].v3());
				
				glTexCoord2d(0,1);
				glVertex3fv(tree->shadowpos[i][3].v3());
				
				glTexCoord2d(0,0);
				glVertex3fv(tree->shadowpos[i][0].v3());
			glEnd();
			
			glPopMatrix();
		}
	}
}

void TREES::BuildCompositeShadow()
{
	SDL_Surface * ts[NUM_TREE_TYPES];
	SDL_Surface * cst;
	
	int t;
	char treechar[256];
	for (t = 0; t < NUM_TREE_TYPES; t++)
	{
		sprintf(treechar, "objects/tree%d_shadow.png", t);
		ts[t] = IMG_Load(settings.GetFullDataPath(treechar).c_str());
	}
	
	char filename[256];
	sprintf(filename, "textures/%s/trees/empty.png", settings.GetTexSize().c_str());
	cst = IMG_Load(settings.GetFullDataPath(filename).c_str());
	
	if (!cst)
	{
		error_log << "Can't open " << settings.GetFullDataPath("textures/" + settings.GetTexSize() + "/trees/empty.png") << endl;
	}
	else
	{
		int sw = cst->w;
		int sh = cst->h;
		//int w = tree_map->w;
		//int h = tree_map->h;
		float t_xoff = terrain.GetOffset().x;
		float t_zoff = terrain.GetOffset().z;
		float t_w = terrain.GetSize().x;
		float t_h = terrain.GetSize().z;
		
		float treescale_x = 2.0;
		float treescale_y = 0.8;
		float treeoffset_x = -0.3;
		float treeoffset_y = 0.0;
		
		TREENODE * curpos = tree_list;
		while (curpos != NULL)
		{
			for (t = 0; t < curpos->numtrees; t++)
			//for (t = 0; t < 1; t++)
			{
				float newscale = (curpos->scale[t]-1.0)*0.75+1.0;
				int tradiusy = (int)((2.0/t_h)*treescale_y*sh*newscale*tree_model[curpos->type][2].GetRadiusXZ());
				int tradiusx = (int)((2.0/t_w)*treescale_x*sw*newscale*tree_model[curpos->type][2].GetRadiusXZ());
				if (tradiusx < 1)
					tradiusx = 1;
				if (tradiusy < 1)
					tradiusy = 1;
				//int tradiusy = (int)((2.0/t_h)*sh*tree_model[curpos->type][2].GetRadiusXZ());
				//int tradiusx = (int)((2.0/t_w)*sw*tree_model[curpos->type][2].GetRadiusXZ());
				int y, x;
				for (y = 0; y < tradiusy*2; y++)
				{
					for (x = 0; x < tradiusx*2; x++)
					{
						int px, py;
						px = (int) (((curpos->tree_ent[t].x - t_xoff)/t_w)*sw+treeoffset_x*tradiusx);
						py = (int) (((curpos->tree_ent[t].z - t_zoff)/t_h)*sh+treeoffset_y*tradiusy);
						int iw, ih;
						iw = ts[curpos->type]->w;
						ih = ts[curpos->type]->h;
						
						int chans = ts[curpos->type]->format->BytesPerPixel;
						int selchan = chans - 1;
						
						px = px - tradiusx + x;
						py = py - tradiusy + y;
						
						int ix, iy;
						ix = (int)(((float) x / (tradiusx*2.0))*iw);
						iy = (int)(((float) y / (tradiusy*2.0))*ih);
						
						if (px < 0)
							px = 0;
						if (py < 0)
							py = 0;
						if (ix < 0)
							ix = 0;
						if (iy < 0)
							iy = 0;
						if (px >= cst->w)
							px = cst->w;
						if (py >= cst->h)
							py = cst->h;
						if (ix >= ts[curpos->type]->w)
							ix = ts[curpos->type]->w;
						if (iy >= ts[curpos->type]->h)
							iy = ts[curpos->type]->h;
						
						
						unsigned char v = ((unsigned char *)(ts[curpos->type]->pixels))[(ix + iw*iy)*chans+selchan];
						//v = 255 - v;
						int cutoff = 150;
						if ((int)(((unsigned char *)(cst->pixels))[px + sw*py]) - (int)v < cutoff)
							((unsigned char *)(cst->pixels))[px + sw*py] = cutoff;
						else
							((unsigned char *)(cst->pixels))[px + sw*py] = ((unsigned char *)(cst->pixels))[px + sw*py] - v;
					}
				}
			}
			
			curpos = curpos -> next;
		}
		
		//glGenTextures(1, &composite_shadow_texture);
		
		glBindTexture(GL_TEXTURE_2D, composite_shadow_texture);
		  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  		  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE, cst->w, cst->h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, cst->pixels );
		
		comploaded = true;
		
		//SDL_SaveBMP(cst, "cst.bmp");
		
		SDL_FreeSurface(cst);
	}
	
	for (t = 0; t < NUM_TREE_TYPES; t++)
		SDL_FreeSurface(ts[t]);
}
