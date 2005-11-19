#include "mouse.h"

void MOUSE::Update(CAMERA & cam, int screenw, int screenh, float timefactor, float fps)
{
	int mousePos_x,mousePos_y;
    int middleX = screenw  >> 1;               // This is a binary shift to get half the width
    int middleY = screenh >> 1;               // This is a binary shift to get half the height
    	
	// Get the mouse's current X,Y position
    SDL_GetMouseState(&mousePos_x,&mousePos_y);
    
    // If our cursor is still in the middle, we never moved... so don't update the screen
    //if( (mousePos_x == middleX) && (mousePos_y == middleY) ) return;

    // Set the mouse position to the middle of our window
    SDL_WarpMouse(middleX, middleY);
	
	int angleY = 0;                            // This is the direction for looking up or down
    int angleZ = 0;                            // This will be the value we need to rotate around the Y axis (Left and Right)
    	
	// Get the direction the mouse moved in, but bring the number down to a reasonable amount
    angleY = ( (middleX - mousePos_x) );        
    angleZ = ( (middleY - mousePos_y) );
	

	/*if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(2))
	{
		if (!mousebounce)
		{
			mousesteering = !mousesteering;
			mousebounce = true;
		}
	}
	else
		mousebounce = false;*/
	
	/*if (mousesteering)
		UpdateSteering(angleY, angleZ);
	else*/
    	UpdateCamera(cam, angleY, angleZ, timefactor, fps);
}

MOUSE::MOUSE()
{
	mousezoom = 0.5;
	mousesteering = settings.GetMouseEnabled();
	mouse_sensitivity_x = settings.GetMouseXSens(); 
	mouse_sensitivity_y = settings.GetMouseYSens();
	deadzone_y = settings.GetMouseYDead();
	steer_x = 0;
	steer_y = 0;
	mousebounce = false;
	ClearBounce();
}

void MOUSE::UpdateCamera(CAMERA & cam, int x, int y, float timefactor, float fps)
{
	static float currentRotX = 0.0f, currentRotY = 0.0f;
	
	float angleY = (float)x / 500.0f;
	float angleZ = (float)y / 500.0f;
	
    // Here we keep track of the current rotation (for up and down) so that
    // we can restrict the camera from doing a full 360 loop.
    currentRotX -= angleZ;
	currentRotY -= angleY;
	
	double temp_radius = sqrt(cam.position.x*cam.position.x+cam.position.z*cam.position.z+(cam.position.y+EARTH_RADIUS)*(cam.position.y+EARTH_RADIUS));
	double temp_long = atan2((double)(cam.position.y+EARTH_RADIUS),(double)cam.position.x);
	double temp_lat = acos(cam.position.z/temp_radius);
	temp_lat -= 1.570796f;
	temp_long -= 1.570796f;
	
	VERTEX tpos = cam.position;
	tpos.y += EARTH_RADIUS;
	tpos = tpos.normalize();

	float wrapat = 3.141592f*2.0f;
	if (currentRotY > wrapat)
		currentRotY -= wrapat;
	if (currentRotY < -wrapat)
		currentRotY += wrapat;
	
    // If the current rotation (in radians) is greater than pi/2 ish, we want to cap it.
    if(currentRotX > 1.4f)
        currentRotX = 1.4f;
    // Check if the rotation is below -1.0, if so we want to make sure it doesn't continue
    else if(currentRotX < -1.4f)
        currentRotX = -1.4f;
    // Otherwise, we can rotate the view around our position
    //else
	
	// To find the axis we need to rotate around for up and down
	// movements, we need to get a perpendicular vector from the
	// camera's view vector and up vector.  This will be the axis.
	//CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
	//vAxis = Normalize(vAxis);
	VERTEX upvector, rightvector;
	upvector.x = 0; upvector.y = 1; upvector.z = 0;
	rightvector.x = 1; rightvector.y = 0; rightvector.z = 0;
	//VERTEX curangley = cam.dir.RotateVec(upvector);
	//VERTEX vAxis = curangley.cross(upvector);

	// Rotate around our perpendicular axis and along the y-axis
	//RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
	//RotateView(angleY, 0, 1, 0);
	
	//cam.Rotate(-angleZ, vAxis.x, vAxis.y, vAxis.z);
	//cam.Rotate(-angleZ, 1, 0, 0);
	//cam.Rotate(-angleY, 0,1,0);
	//cam.Rotate(-angleY, curangley.x,curangley.y,curangley.z);
	
	//cam.dir.SetEuler(currentRotX, currentRotY, 0);
	
	QUATERNION curveoffset;
	curveoffset.SetEuler(temp_lat, temp_long, temp_lat);
	VERTEX up, posx, posz;
	up.y = 1.0f;
	posx.x = 1.0f;
	posz.z = 1.0f;
	VERTEX newup = curveoffset.RotateVec(up);
	VERTEX newx = curveoffset.RotateVec(posx);
	VERTEX newz = curveoffset.RotateVec(posz);
	/*newx.DebugPrint();
	newup.DebugPrint();
	tpos.DebugPrint();*/
	
	QUATERNION rightangle;
	rightangle.SetAxisAngle(1.570796f, 0,0,1);
	newx = rightangle.RotateVec(tpos);
	//newx.DebugPrint();
	//cout << endl;
	
	QUATERNION cor, qx, qy, qz;
	
	cor.SetAxisAngle(temp_long, 0, 0, 1);
	
	qx.SetAxisAngle(currentRotX, 1, 0, 0);
	
	qy.SetAxisAngle(currentRotY, -tpos.x, tpos.y, -tpos.z);
	//qy.SetAxisAngle(currentRotY, 0.1, 0.9, 0);
	//qy.SetAxisAngle(currentRotY, 0, 1, 0);
	//cout << currentRotX << endl;
	//cout << currentRotY << endl;
	//qz.SetAxisAngle(temp_lat+temp_long, newz.x, newz.y, newz.z);
	//qz.SetAxisAngle(0.2f, newz.x, newz.y, newz.z);
	
	
	viewdir = cor*qx*qy*qz;
	
	//cam.dir.SetEuler(currentRotX-temp_lat, currentRotY+temp_long, 0);
	//cam.dir.SetEuler(-temp_lat, currentRotY+temp_long*2.0f, 0);
	
	if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
	{
		mousezoom += ZOOMSPEED * timefactor/fps;
		if (mousezoom > 1.0f)
			mousezoom = 1.0f;
	}
	
	if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(3))
	{
		mousezoom -= ZOOMSPEED * timefactor/fps;
		if (mousezoom < 0.0f)
			mousezoom = 0.0f;
	}
}

void MOUSE::UpdateSteering(int x, int y)
{
	float fx = ((float) x*mouse_sensitivity_x) / 1500.0;
	float fy = ((float) y*mouse_sensitivity_y) / 250.0;
	
	steer_x += fx;
	steer_y += fy;
	
	if (steer_x > 1.0)
		steer_x = 1.0;
	if (steer_y > 1.0)
		steer_y = 1.0;
	
	if (steer_x < -1.0)
		steer_x = -1.0;
	if (steer_y < -1.0)
		steer_y = -1.0;
	
	//cout << steer_x << "," << steer_y << endl;
}

bool MOUSE::GetMouseControls(float * x, float * y, bool * click_l, bool * click_r)
{
	*x = steer_x;
	*y = steer_y;
	
	if (steer_y >= 0 && steer_y < deadzone_y)
		*y = 0;
	else if (steer_y < 0 && steer_y > -deadzone_y)
		*y = 0;
	else
	{
		if (steer_y < 0)
			*y = (steer_y + deadzone_y)*(1.0/(1.0-deadzone_y));
		else
			*y = (steer_y - deadzone_y)*(1.0/(1.0-deadzone_y));
	}
	
	rbounce = (SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1));
	lbounce = (SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(3));
	
	*click_l = (lbounce && !lastl);
	*click_r = (rbounce && !lastr);

	//ClearBounce();
	lastl = lbounce;
	lastr = rbounce;
	
	return mousesteering;
}
