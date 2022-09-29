
#ifndef __LINHA__
#define __LINHA__

#ifdef WIN32
#include <windows.h>
#include "gl\glut.h"
#endif


#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#include "Ponto.h"

class Linha {
	
public:
    Linha();
    Linha(float x1,float y1,float x2,float y2);
	float x1,y1,x2,y2;
    float minx,miny, maxx, maxy; // envelope
    Ponto Meio, MeiaLargura;

    void geraLinha(int limite, int TamMax);
	void desenhaLinha();
    void GeraEnvelope();
    bool EnvelopesColidem(Linha L);

};
#endif
