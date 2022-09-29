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
    int cor;
    float Velocidade;
    float tAtual;
    int direcao; // andando do fim para o inicio, ou ao contrario

    void desenha();
    void AtualizaPosicao(float tempoDecorrido);
    Ponto ObtemPosicao();
};


#endif /* Instancia_hpp */
