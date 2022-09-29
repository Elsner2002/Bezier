//
//  CurvaBezier.hpp
//  OpenGL
//
//  Created by Márcio Sarroglia Pinho on 10/10/21.
//  Copyright © 2021 Márcio Sarroglia Pinho. All rights reserved.
//

#ifndef Bezier_h
#define Bezier_h

#include <iostream>
using namespace std;


#ifdef WIN32
#include <windows.h>
#include <glut.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <GL/glut.h>
#endif

#include "Ponto.h"

class Bezier
{
    Ponto Coords[3];
    //int Indices[3];

public:
    float ComprimentoTotalDaCurva;
    int cor;
    
    Bezier();
    Bezier(Ponto P0, Ponto P1, Ponto P2);
    Bezier(Ponto V[]);
    Ponto Calcula(double t);
    Ponto getPC(int i);
    void Traca();
    double CalculaT(double distanciaPercorrida);
    void calculaComprimentoDaCurva();
};

#endif 
