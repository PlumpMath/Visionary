#include "LoadTexture.h"

#include <stdafx.h>
#include <stdio.h>


bool GetParametersFromFilename( char * filename, GLuint &width, GLuint &height, GLuint &depth)
{
	char* rgba = "RGBA", *rgb4 = "RGB4";
	GLint state = 0;

	// find the first token!
	char *p = strchr(filename, '_');
	while( p != NULL )
	{
		p += 1;

		// _\d+[Ww]_\d+[Hh]_RGB[A].raw

		int num = atoi(p); // convert until not valid number
		
		// if it's a RAW format the parameters are stored in the file name
		switch(state++)
		{
			case 0: // get width
				width = num;
				break;
			case 1:	// get height
				height = num;
				break;
			case 2: // get depth
				if( *(long*)p == *(long*)rgba )
				{
					depth = GL_RGBA;
				}
				else if( *(long*)p == *(long*)rgb4 )
				{
					depth = GL_RGB4;
				}
				else
				{
					depth = GL_RGB;
				}
				break;
		}

		p = strchr(p,'_');

	}

	return (state >= 3);
#undef CREATESHORT
}


bool LoadTextureRaw( BYTE * &data, const char * filename, GLuint &width, GLuint &height, GLuint &depth, GLuint &size, GLuint &type, GLuint &wrap )
{
	FILE * file;

	// open texture data
    file = fopen( filename, "rb" );
    if ( file == NULL ) return false;

	switch(depth)
	{
	case GL_RGBA: size = 4; break;
	case GL_RGB: 
	case GL_RGB4: size = 3; break;
	}

	type = GL_UNSIGNED_BYTE; // for raw this is typical

	// allocate buffer
	int lBuffer = width * height * size;
    data = (BYTE*)malloc( lBuffer );

    // read texture data
    fread( data, lBuffer, 1, file );

    fclose( file );

	return true;
}

bool LoadTextureIntoGL( GLuint &iTexture, BYTE* data, GLuint width, GLuint height, GLuint depth, GLuint size, GLuint type, int wrap )
{
    // allocate a texture name
    glGenTextures( 1, &iTexture);

    // select our current texture
    glBindTexture( GL_TEXTURE_2D, iTexture );

    // select modulate to mix texture with color for shading
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                     GL_LINEAR_MIPMAP_NEAREST );
    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                     wrap ? GL_REPEAT : GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                     wrap ? GL_REPEAT : GL_CLAMP );

    // build our texture mipmaps
    gluBuild2DMipmaps( GL_TEXTURE_2D, size, width, height,
                       depth, type, data );

    return true;
}

bool LoadTexture( GLuint &glTexId )
{
	GLuint width, height, depth, size, type, wrap = 0;
	char *filename = "C:\\Documents and Settings\\Robert Wlaschin\\Desktop\\test_128W_128h_RGB.raw";

	bool bRet = GetParametersFromFilename( filename, width, height, depth);
	if( bRet )
	{
		BYTE *data = NULL;
		bRet = LoadTextureRaw( data, filename, width, height, depth, size, type, wrap );
		if( bRet )
		{
			bRet = LoadTextureIntoGL( glTexId, data, width, height, depth, size, type, wrap );
			free(data);
		}

		return bRet;
	}

	return false;
}