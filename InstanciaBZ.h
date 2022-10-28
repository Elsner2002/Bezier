//
//  Instancia.h
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 22/09/20.
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//


#ifndef Instancia_hpp
#define Instancia_hpp
#include <iostream>
using namespace std;

#include "Bezier.h"
#include "ListaDeCoresRGB.h"
#include "Poligono.h"

typedef void TipoFuncao();

class InstanciaBZ{

public:
    InstanciaBZ();
    InstanciaBZ(Bezier *Curva);

    TipoFuncao *modelo; // Modelo

    Bezier *Curva; // referencia para a curva onde esta' a instancia
    Ponto Posicao, Escala;
    float Rotacao;

    int nroDaCurva;
    int proxCurva;
	bool indoParaZ;
    int cor;
    float Velocidade;
    float tAtual;

    vector<int> curvasLigadas;
    bool listaCurvasPos;
    int curvaListaCurvas;

    void desenha();
    void AtualizaPosicao(float tempoDecorrido);
    Ponto ObtemPosicao();
	void AtualizaIndoParaZ(Bezier *proxCurva);
};


#endif /* Instancia_hpp */
