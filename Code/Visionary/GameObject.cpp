
#include "stdafx.h"
#include "GameObject.h"

//////////////////////////////////////////////////////////////////

// What do I need for lighting?
// Eight slots
// Each slot is a stack with the current light at the top

// Adding a light should push on to a stack (automatic, or choice 1-8 is valid 0 is auto) and initialize light
// Removing light should pop light off and initialize previous light, if no light, disables light

//////////////////////////////////////////////////////////////////

CGameObject::CGameObject( SGlobalData &data ):
	m_hRC(data.hRC),m_hDC(data.hDC),m_hWnd(data.hWnd), m_hInstance(data.hInstance),
		m_nMaxObjects(MAXINSTANCES), m_nObjectList(0), m_pObjectList(m_pObjectPtr), 
		m_nSearchMaxObjects(MAXINSTANCES),m_nSearchObjectList(0),m_pSearchObjectList(m_pSearchObjects),
		m_nCameraList(0), m_nMaxCameras(MAXCAMERAS)
{
		memset(m_pSearchObjects,0,sizeof(m_pSearchObjects));
}

bool CGameObject::AddObject( SSearchObject* pObject )
{
	if( m_nSearchObjectList < m_nSearchMaxObjects )
	{		
		if( pObject )
		{
			SSearchObject *pSearchObject = m_pSearchObjectList + m_nSearchObjectList++;

			memcpy( pSearchObject, pObject, sizeof(*pObject));

			return true;
		} 

		DBGPRINT( (msg, "Error: Instance object passed in to AddObject was NULL\n") );
		return false;
	}

	DBGPRINT( (msg, "Error: Reached maximum number of concurrent SearchObjects (you may not have wanted over %d?)\n", MAXINSTANCES) );
	return false;
}

bool CGameObject::AddObject( VisInstance* pObject )
{
	if( m_nObjectList < m_nMaxObjects )
	{		
		if( pObject )
		{
			VisInstancePtr *pInstancePtr = m_pObjectList + m_nObjectList++;

			pInstancePtr->type = pObject->header.data.id.type;
			pInstancePtr->ptr = pObject;
		
			return true;
		} 

		DBGPRINT( (msg, "Error: Instance object passed in to AddObject was NULL\n") );
		return false;
	}

	DBGPRINT( (msg, "Error: Reached maximum number of concurrent Instances (you may not have wanted over %d?)\n", MAXINSTANCES) );
	return false;
}

bool Push( unsigned long* stackbuf, unsigned long*&stackhead, unsigned long item, int &nstack, int maxstack )
{
	if( nstack < maxstack )
	{
		stackbuf[nstack] = item;
		stackhead = (unsigned long*)(stackbuf[nstack]);
		nstack++;

		return true;
	}

	return false;
}

bool Pop( unsigned long* stackbuf, unsigned long*&stackhead, int &nstack )
{
	if( nstack > 0 )
	{
		nstack--;
		stackbuf[nstack] = 0;
		stackhead = (unsigned long*)(stackbuf[nstack-1]);

		return true;
	}

	return false;
}

unsigned long Top( unsigned long* stackbuf, unsigned long*&stackhead, int &nstack )
{
	if( nstack > 0 )
	{
		return stackbuf[nstack-1];
	}

	return NULL;
}

bool CGameObject::PushCamera( VisInstance* pCamera )
{
	return Push( (unsigned long*) m_pCamera,
				 (unsigned long*&)m_pCameraList, 
				(unsigned long)   pCamera, 
						  m_nCameraList, 
						  m_nMaxCameras );
}

bool CGameObject::PopCamera( void )
{
	return Pop( (unsigned long*)m_pCamera,(unsigned long*&)m_pCameraList, m_nCameraList );
}

VisInstance* CGameObject::TopCamera( void )
{
	return (VisInstance*)Top( (unsigned long*)m_pCamera,(unsigned long*&)m_pCameraList, m_nCameraList );
}

visint CGameObject::CountCamera( void ) { return m_nCameraList; }

