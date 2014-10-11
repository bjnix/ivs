#include "../include/Window.h"
#define PLANE_BIAS 10

/*=============================================================================
	Viewer implementation
=============================================================================*/
Viewer::Viewer(Client* viconClient, string name){
	_client = viconClient;
	_viewerName = name;

	Position = ViVec(0.0);
	LookDirection = ViVec(0.0);
}
//-----------------------------------------------------------------------------
void Viewer::Update(){
	// calculate our new position
	Position = getCenterOfMass();

	// get the direction the viewer is looking
	// TODO transform a forward vector such as (0, 0, 1) by the rotation matrix
	// which can be retrieved from the API instead.
	Output_GetMarkerGlobalTranslation posData = _client->GetMarkerGlobalTranslation(_viewerName, "Forward");

	if(posData.Translation[0] != 0){
		ViVec front = ViVec(posData.Translation);
		LookDirection = front - Position;
		LookDirection = normalize(LookDirection);
	}
}
//-----------------------------------------------------------------------------
ViVec Viewer::getCenterOfMass(){
	int markerCount = _client->GetMarkerCount(_viewerName).MarkerCount;
	int visible = 0;
	ViVec sum = ViVec(0.0);

	for(int i = markerCount; i--;){
		string markerName = _client->GetMarkerName(_viewerName, i).MarkerName;
				Output_GetMarkerGlobalTranslation posData = 
				_client->GetMarkerGlobalTranslation(_viewerName, markerName);

		// don't factor in occluded markers
		if(!posData.Occluded){
			sum += ViVec(posData.Translation);
			++visible;
		}
	}

	// average the visible
	sum /= visible;

	// finally, set the position to the center of mass
	Position = sum;

	return Position;
}
//-----------------------------------------------------------------------------
void Viewer::SetLookPos(ViVec pos){
	_windowPos = pos;
}
/*=============================================================================
	Window class implementation
  ===========================================================================*/
Window::Window(Client *viconClient, string windowName){
	_client = viconClient;
	RecalculateWindow(windowName);
}
//-----------------------------------------------------------------------------
ViVec Window::getMarker(string subject, int index){
	string markerName = _client->GetMarkerName(subject, index).MarkerName;
	Output_GetMarkerGlobalTranslation posData = _client->GetMarkerGlobalTranslation(subject, markerName);
	return ViVec(posData.Translation);
}
//-----------------------------------------------------------------------------
ViVec Window::getMarker(string subject, string name){
	Output_GetMarkerGlobalTranslation posData = _client->GetMarkerGlobalTranslation(subject, name);
	return ViVec(posData.Translation);
}
//-----------------------------------------------------------------------------
void Window::RecalculateWindow(string name){

	// keep the name of the window
	_windowName = name;

	std::cout << "Subject: " << name << "\n";

	// get each marker
	ViVec left  = this->getMarker(name, "Left");
	ViVec right = this->getMarker(name, "Right");
	ViVec top   = this->getMarker(name, "Top");
	ViVec bottom= this->getMarker(name, "Bottom");

	do{
		// Get the first frame of data so we can set stuff up initially
		while(_client->GetFrame().Result != Result::Success ){
			// Sleep a little so that we don't lumber the CPU with a busy poll
			sleep(1);
		}
		// get each marker
		left  = this->getMarker(name, "Left");
		right = this->getMarker(name, "Right");
		top   = this->getMarker(name, "Top");
		bottom= this->getMarker(name, "Bottom");
	}while(left.x == 0 || right.x == 0 || top.x == 0 || bottom.x == 0);

	std::cout << "left " << left << "\n";
	std::cout << "right " << right << "\n";
	std::cout << "top " << top << "\n";
	std::cout << "bottom " << bottom << "\n";

	double screenWidth = length(left - right) / 4.0; 
	double screenHeight = length(top - bottom) / 2.0;

	// dimensions of one screen
	ViVec screen((double)screenWidth, (double)screenHeight, 0.0);

	printf("width %f height %f\n", screenWidth, screenHeight);

	// calculated directions
	ViVec leftDir = _left = normalize(left - right);
	ViVec upDir   = _up = normalize(top - bottom);

	// calculate the min and max points on the window (and center)
	_min = _corners[0] = left + (leftDir * screen.x) + (upDir * (-screen.y * 2));
	_corners[1] = right + (leftDir * -screen.x) + (upDir * (-screen.y * 2));
	_max = _corners[2] = right + (leftDir * -screen.x) + (upDir * screen.y * 2);
	_corners[3] = left + (leftDir * screen.x) + (upDir * screen.y * 2);

	_center = (_min + _max) / 2.0; 

	// calculate the normal of the window
	_normal = -cross(leftDir, upDir);

	std::cout << "Min " << _min << " Max " << _max << "\n";
}
//-----------------------------------------------------------------------------
Viewer* Window::AddViewer(string name){
	Viewer* v = new Viewer(_client, name);
	Viewers.push_back(v);

	return v;
}
//-----------------------------------------------------------------------------
void Window::EvaluateViewers(){
	for(int i = Viewers.size(); i--;){
		Viewer* v = Viewers[i];

		// get their new positions and look directions
		v->Update();

		// solve for intersection point
		float t = 0;
		Ray r = {v->Position, v->LookDirection};
		ViVec lp = InterPlane(r, _center, _normal, &t);
		v->SetLiteralLook(lp);

		// make sure the viewer is looking at the screne
		if(t > 0){
			double BIAS = PLANE_BIAS;
			double minX = _min.x, minY = _min.y, minZ = _min.z;
			double maxX = _max.x, maxY = _max.y, maxZ = _max.z;

			if(_min.x > _max.x){
				minX = _max.x;
				maxX = _min.x;
			}

			if(_min.y > _max.y){
				minY = _max.y;
				maxY = _min.y;
			}

			if(_min.z > _max.z){
				minZ = _max.z;
				maxZ = _min.z;
			}

			v->LookingAtScreen = false;

			if(lp.x >= maxX + BIAS || lp.x <= minX - BIAS) continue;
			if(lp.y >= maxY + BIAS || lp.y <= minY - BIAS) continue;
			if(lp.z >= maxZ + BIAS || lp.z <= minZ - BIAS) continue;

			v->LookingAtScreen = true;
			v->SetLookPos(lp);
		}
		else{
			// otherwise, indicate that they are not
			v->LookingAtScreen = false;
		}
	}
}
