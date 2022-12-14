//
//  InstanciaBZ.cpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 22/09/20.
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//

#include "InstanciaBZ.h"
#include <cmath>

// ***********************************************************
//  void InstanciaPonto(Ponto3D *p, Ponto3D *out)
//  Esta funcao calcula as coordenadas de um ponto no
//  sistema de referencia do universo (SRU), ou seja,
//  aplica as rotacoes, escalas e translacoes a um
//  ponto no sistema de referencia do objeto (SRO).
// ***********************************************************
void InstanciaPonto(Ponto &p, Ponto &out)
{
    GLfloat ponto_novo[4];
    GLfloat matriz_gl[4][4];
    int  i;

    glGetFloatv(GL_MODELVIEW_MATRIX,&matriz_gl[0][0]);

    for(i=0;i<4;i++)
    {
        ponto_novo[i]= matriz_gl[0][i] * p.x+
        matriz_gl[1][i] * p.y+
        matriz_gl[2][i] * p.z+
        matriz_gl[3][i];
    }
    out.x=ponto_novo[0];
    out.y=ponto_novo[1];
    out.z=ponto_novo[2];
}

Ponto InstanciaPonto(Ponto P)
{
    Ponto temp;
    InstanciaPonto(P, temp);
    return temp;
}

InstanciaBZ::InstanciaBZ()
{

    Rotacao = 0;
    Posicao = Ponto(0,0,0);
    Escala = Ponto(1,1,1);

    nroDaCurva = 0;
    proxCurva = -1;
    tAtual = 0.0;
    indoParaZ = true;
    Velocidade = 1;
}

InstanciaBZ::InstanciaBZ(Bezier *C)
{

    Rotacao = 0;
    Posicao = Ponto(0,0,0);
    Escala = Ponto(1,1,1);

    Curva = C;
    tAtual = 0;
	indoParaZ = true;
}

void InstanciaBZ::desenha()
{
    // Aplica as transformacoes geometricas no modelo
    glPushMatrix();
        glTranslatef(Posicao.x, Posicao.y, 0);
        glRotatef(Rotacao, 0, 0, 1);
        glScalef(Escala.x, Escala.y, Escala.z);

        defineCor(cor);
        (*modelo)(); // desenha a instancia

    glPopMatrix();
}

Ponto InstanciaBZ::ObtemPosicao()
{
    // aplica as transformacoes geometricas no modelo
    // desenha a geometria do objeto

    glPushMatrix();
        glTranslatef(Posicao.x, Posicao.y, 0);
        glRotatef(Rotacao, 0, 0, 1);
        Ponto PosicaoDoPersonagem;
        Ponto Origem (0,0,0);
        InstanciaPonto(Origem, PosicaoDoPersonagem);
        //PosicaoDoPersonagem.imprime(); cout << endl;
    glPopMatrix();
    return PosicaoDoPersonagem;
}

void InstanciaBZ::AtualizaPosicao(float tempoDecorrido)
{
    if(parado){
        return;
    }
    double dist = Velocidade*tempoDecorrido;
	double deltaT = Curva->CalculaT(dist);
	tAtual += deltaT;

	if (tAtual >= 1.0) {
		tAtual = 0;
		nroDaCurva = proxCurva;
		proxCurva = -1;
		return;
	}

	double tUsado = indoParaZ ? tAtual : 1 - tAtual;
    Ponto posAntiga = Posicao;//usa na rotacao
    Posicao = Curva->Calcula(tUsado);
    
    //rotacao
    Ponto vetHor (1,0,0);
    Ponto vetMov = Posicao - posAntiga;
    double cosAngulo = ProdEscalar(vetHor,vetMov)/vetMov.modulo();
    double arcos = acosf(cosAngulo);
    if(vetMov.y<0){
        arcos=arcos*-1;
    }
    Rotacao=arcos*(180/M_PI); 
    //rotacao
}

void InstanciaBZ::AtualizaIndoParaZ(Bezier *proxCurva)
{
	if (
		this->Curva->getPC(2) == proxCurva->getPC(0)
		|| this->Curva->getPC(0) == proxCurva->getPC(0)
	) {
		this->indoParaZ = true;
	} else {
		this-> indoParaZ = false;
	}
}
