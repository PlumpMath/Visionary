
#include "BuildObject.h"

static int nInstance = 0;
static int nCamera = 0;

bool BuildObject_Instance( VisInstance &instance, visint size, VisInstance *Attach, bool asChild )
{
	// Reset data
	memset(&instance, 0, sizeof(instance));

	// Initialize header data
	instance.header.data.size = (size<sizeof(instance))?sizeof(instance):size;
	instance.header.data.id.number = ++nInstance;
	instance.header.data.id.type = VIT_INSTANCE;
	instance.header.id = instance.header.data.id;

	if(Attach)
	{
		if(asChild)
		{
			Attach->child.ptr = &instance;
			Attach->child.type = instance.header.id.type;
		}
		else
		{
			Attach->sibling.ptr = &instance;
			Attach->sibling.type = instance.header.id.type;
		}
	}

	return true;
}

bool BuildObject_Camera( VisCamera &camera, bool asOrtho, visfloat fNear, visfloat fFar, visfloat fAspect, visfloat fRov)
{
	// Change this to create arrays that can be altered by a matrix
	memset(&camera, 0, sizeof(camera));

	camera.header.data.size = sizeof(camera);
	camera.header.id.number = ++nCamera;
	camera.header.id.type = VIT_CAMERA;
	camera.header.data.id = camera.header.id;

	camera.iType = asOrtho;
	camera.fNear = fNear;
	camera.fFar	= fFar;
	camera.fAspect = fAspect;
	camera.fCustom = fRov;

	return true;
}

bool BuildObject_Light( VisLight &light, visint size, vislong lightid )
{
	light.header.data.size = size;
	light.header.data.id.number = 
	light.header.data.id.type = VIT_LIGHT;
	light.header.id = light.header.data.id;

	light.lightid = 0;

	return true;
}
