#pragma once

#include "TypeDefinitions.h"

extern struct SGlobalData g_GlobalData;
extern class  CGameObject *gp_GameObject;

// class CObject;

#define MAXINSTANCES (1000)
#define MAXCAMERAS (32)
#define MAXLIGHTS (64)

// Define the main Game Object here.
class CGameObject
{
	visint m_nSearchMaxObjects;
	SSearchObject m_pSearchObjects[MAXINSTANCES];
	visint m_nMaxObjects;
	VisInstancePtr  m_pObjectPtr[MAXINSTANCES]; // hardcoded for the moment
	visint m_nMaxCameras, m_nCameraList;				// Length of list
	VisInstance*  m_pCamera[MAXINSTANCES]; // hardcoded for the moment
	
public:

	HGLRC           &m_hRC;				// Permanent Rendering Context
	HDC             &m_hDC;				// Private GDI Device Context
	HWND            &m_hWnd;			// Holds Our Window Handle
	HINSTANCE       &m_hInstance;		// Holds The Instance Of The Application

	VisInstancePtr* m_pObjectList;		// Pointer to first object in the scene
	visint m_nObjectList;				// Length of list

	SSearchObject* m_pSearchObjectList;	// Pointer to first object in the scene
	visint m_nSearchObjectList;			// Length of list

	// Acts like stack
	VisInstance* m_pCameraList;		// Pointer to last camera

	VisInstancePtr m_PlayerObject;

	/*std::vector<VisInstance*> vCollisionList;
	std::vector<VisInstance*> vPhysicsList;
	std::vector<VisInstance*> vAnimationList;
	std::vector<VisInstance*> vUserList;
	std::vector<VisInstance*> vAiList;*/

	CGameObject( SGlobalData &data );

	bool AddObject( SSearchObject* pObject );
	bool AddObject( VisInstance* pObject );
	
	bool PushCamera( VisInstance* pCamera );
	bool PopCamera( void );
	VisInstance* TopCamera( void );
	visint CountCamera( void );
};

