#pragma once
#ifndef IVS_WINDOW
#define IVS_WINDOW

#include <iostream>
#include <fstream>
#include <cassert>
#include <ctime>
#include <stdlib.h>
#include <vector>
#include <string.h>

#include "Intersection.h"
#include "Client.h"

using namespace ViconDataStreamSDK::CPP;
using namespace std;

class Viewer{
public:
	Viewer(Client *viconClient, string viewerName);
	void Update();
	void SetLookPos(ViVec pos);
	ViVec GetLookPos(){ return _windowPos; }
	void SetLiteralLook(ViVec pos){ _literalLook = pos; }
	ViVec GetLiteralLookPos(){ return _literalLook; } 

	ViVec Position;
	ViVec LookDirection;
	bool LookingAtScreen;
private:
	Client* _client;
	ViVec _windowPos, _literalLook;
	string _viewerName;

	ViVec getCenterOfMass(); // TODO improve this, averaging may contribute to noise

	ViVec getMarker(string subject, string name){
		write(1, "merr\n", 5);
		Output_GetMarkerGlobalTranslation posData = _client->GetMarkerGlobalTranslation(subject, name);
		std::cout << "Marker: " << name << " --> (" << posData.Translation[0] << ", " << posData.Translation[1] << ", "<< posData.Translation[2] <<")\n";
		return ViVec(posData.Translation);
	}
};

/*---------------------------------------------------------------------------*/

class Window{
public:
	// list of tracked viewers
	vector<Viewer*> Viewers;

	Window(Client *viconClient, string windowName);
	
	Viewer* AddViewer(string name);

	void RecalculateWindow(string name);
	void EvaluateViewers();

	// returns the lower right corner of the wall
	ViVec GetMax(){ return _max; }
	// returns the upper left corner of the wall
	ViVec GetMin(){ return _min; }
	// returns the center of the screen in tracked space
	ViVec GetCenter(){ return _center;}
	// returns the normalized normal projected out of the wall
	ViVec GetNormal(){ return _normal; }
	// returns the normalized up vector (from center to 'top' marker)
	ViVec GetUp(){ return _up; }
	// returns the normalized left vector (from center to 'left' marker)
	ViVec GetLeft(){ return _left; }
	// returns pointer to 4 element array of each corner on the wall
	ViVec* GetCorners(){ return _corners; }
private:
	Client* _client;
	string _windowName;
	ViVec _normal, _up, _left;
	ViVec _min, _max;
	ViVec _center;
	ViVec _corners[4];

	ViVec getMarker(string subject, int index);
	ViVec getMarker(string subject, string name);
};

#endif
