// **********************************************************************
// PUCRS/Escola Polit�cnica
// COMPUTA��O GR�FICA
//
// Programa basico para criar aplicacoes 2D em OpenGL
//
// Marcio Sarroglia Pinho
// pinho@pucrs.br
// **********************************************************************

// Para uso no Xcode:
// Abra o menu Product -> Scheme -> Edit Scheme -> Use custom working directory
// Selecione a pasta onde voce descompactou o ZIP que continha este arquivo.
//

#include <cstdlib>
#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>

using namespace std;

#ifdef WIN32
#include <windows.h>
#include <glut.h>
#else
#include <sys/time.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <GL/glut.h>
#endif

#include "Ponto.h"
#include "Poligono.h"
#include "InstanciaBZ.h"

#include "Temporizador.h"
#include "ListaDeCoresRGB.h"

Temporizador T;
double AccumDeltaT=0;
Temporizador T2;

InstanciaBZ Personagens[11];
Poligono envelopes[11];
bool curvaFoiSelecionada = false;
int curvaSelecionada = 0;

Bezier Curvas[26];
unsigned int nCurvas;

// Limites l�gicos da �rea de desenho
Ponto Min, Max;

//lista de curvas que tem interseccao
bool ligaCurvasX[26][26];
bool ligaCurvasZ[26][26];

bool desenha = false, mover=false;

Poligono Mapa, MeiaSeta, Mastro, CurvasBZ, PontosCurvas;
Poligono envelopeMastro;
int nInstancias=0;

float angulo=0.0;


double nFrames=0;
double TempoTotal=0;

// **********************************************************************
//
// **********************************************************************
void animate()
{
    double dt;
    dt = T.getDeltaT();
    AccumDeltaT += dt;
    TempoTotal += dt;
    nFrames++;

    if (AccumDeltaT > 1.0/30) // fixa a atualiza��o da tela em 30
    {
        AccumDeltaT = 0;
        angulo+=2;
        glutPostRedisplay();
    }
    if (TempoTotal > 5.0)
    {
        cout << "Tempo Acumulado: "  << TempoTotal << " segundos. " ;
        cout << "Nros de Frames sem desenho: " << nFrames << endl;
        cout << "FPS(sem desenho): " << nFrames/TempoTotal << endl;
        TempoTotal = 0;
        nFrames = 0;
    }
}
// **********************************************************************
//  void reshape( int w, int h )
//  trata o redimensionamento da janela OpenGL
// **********************************************************************
void reshape( int w, int h )
{
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define a area a ser ocupada pela area OpenGL dentro da Janela
    glViewport(0, 0, w, h);
    // Define os limites logicos da area OpenGL dentro da Janela
    glOrtho(Min.x,Max.x, Min.y,Max.y, -10,+10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
// **********************************************************************
// **********************************************************************
void DesenhaEixos()
{
    Ponto Meio;
    Meio.x = (Max.x+Min.x)/2;
    Meio.y = (Max.y+Min.y)/2;
    Meio.z = (Max.z+Min.z)/2;

    glBegin(GL_LINES);
    //  eixo horizontal
        glVertex2f(Min.x,Meio.y);
        glVertex2f(Max.x,Meio.y);
    //  eixo vertical
        glVertex2f(Meio.x,Min.y);
        glVertex2f(Meio.x,Max.y);
    glEnd();
}

// **********************************************************************
void DesenhaInstancia()
{
    Mastro.desenhaPoligono();
}

// **********************************************************************
// Esta fun��o deve instanciar todos os personagens do cen�rio
// **********************************************************************
void CriaInstancias()
{
    int nc = rand()%26;
    Personagens[0].modelo = DesenhaInstancia;
    Personagens[0].cor= LimeGreen;
    Personagens[0].indoParaZ = true;
    Personagens[0].Escala = Ponto (0.4,0.4,0.4);
	Personagens[0].Curva = &Curvas[nc];
    Personagens[0].nroDaCurva=nc;
    Personagens[0].Posicao = Personagens[0].Curva->getPC(0);
    Personagens[0].parado=false;
    nInstancias = 11;

	for (size_t i = 1; i < nInstancias; i++) {
        Personagens[i].parado=false;
        nc = rand()%26;
		Personagens[i].Curva = &Curvas[nc];
		Personagens[i].Posicao = Personagens[i].Curva->getPC(0);

        bool existe=false;
        do{
            Personagens[i].cor= rand() % 100;
            for(size_t j=0; j<i;j++){
                if(Personagens[i].cor==Personagens[j].cor){
                    existe=true;
                    break;
                }
                else{
                    existe=false;
                }
            }
        }while(existe);

		Personagens[i].modelo = DesenhaInstancia;
		Personagens[i].indoParaZ = true;
		Personagens[i].Escala = Ponto (0.4,0.4,0.4);
        Personagens[i].nroDaCurva=nc;
	}

	for (size_t i = 0; i < nInstancias; i++) {
		Poligono envelope;
		envelope.insereVertice(Ponto(0, 0));
		envelope.insereVertice(Ponto(0, 0));
		envelope.insereVertice(Ponto(0, 0));
		envelope.insereVertice(Ponto(0, 0));
		envelopes[i] = envelope;
	}
}

void CarregaModelos()
{
    Mapa.LePoligono("EstadoRS.txt");
    MeiaSeta.LePoligono("MeiaSeta.txt");
    Mastro.LePoligono("Triangulo.txt");

	Ponto min, max;
	Mastro.obtemLimites(min, max);
	envelopeMastro.insereVertice(Ponto(min.x, min.y));
	envelopeMastro.insereVertice(Ponto(min.x, max.y));
	envelopeMastro.insereVertice(Ponto(max.x, max.y));
	envelopeMastro.insereVertice(Ponto(max.x, min.y));

    PontosCurvas.LePoligono("cordPontos.txt");
    CurvasBZ.LePoligonoZ("ListaCurvas.txt");
}

void CriaCurvas()
{
    nCurvas = CurvasBZ.getNVertices();

    for(int i =0;i<nCurvas;i++){
        bool existe=false;
        do{
            Curvas[i].cor= rand() % 100;
            for(size_t j=0; j<i;j++){
                if(Curvas[i].cor==Curvas[j].cor){
                    existe=true;
                    break;
                }
                else{
                    existe=false;
                }
            }

        }while(existe);

        Curvas[i] = Bezier(
			PontosCurvas.getVertice(CurvasBZ.getVertice(i).x),
			PontosCurvas.getVertice(CurvasBZ.getVertice(i).y),
			PontosCurvas.getVertice(CurvasBZ.getVertice(i).z)
		);
    }
}

//cria matriz com as curvas que se encontram
void encontroCurvas(){
    for (int i = 0; i < 26; i++) {
        for (int j = 0; j < 26; j++) {
			ligaCurvasZ[i][j] = false;
			ligaCurvasX[i][j] = false;

            if (i != j) {
				if (Curvas[i].getPC(2) == Curvas[j].getPC(2)||Curvas[i].getPC(2) == Curvas[j].getPC(0)) {
					ligaCurvasZ[i][j] = true;
				} else if (Curvas[i].getPC(0) == Curvas[j].getPC(2)||Curvas[i].getPC(0) == Curvas[j].getPC(0)) {
					ligaCurvasX[i][j] = true;
				}
            }
        }
    }
}

// **********************************************************************
//
// **********************************************************************
void init()
{
    // Define a cor do fundo da tela (PRETO)
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    //carrega e cria modelos
    CarregaModelos();
    CriaInstancias();
    CriaCurvas();
    encontroCurvas();

    //cria tela 9X9
    float d = 9;
    Min = Ponto(-d,-d/2);
    Max = Ponto(d,d/2);
}

// **********************************************************************
void DesenhaPersonagens(float tempoDecorrido)
{
    for(size_t i = 0; i < nInstancias; i++) {
        Personagens[i].desenha();
    }
}

void CalculaEnvelope(InstanciaBZ *personagem, size_t i)
{
	glPushMatrix();

	glTranslatef(personagem->Posicao.x, personagem->Posicao.y, 0);
	glRotatef(personagem->Rotacao, 0, 0, 1);
	glScalef(personagem->Escala.x, personagem->Escala.y, personagem->Escala.z);

	Ponto p = envelopeMastro.getVertice(0);
	Ponto q = envelopeMastro.getVertice(1);
	Ponto r = envelopeMastro.getVertice(2);
	Ponto s = envelopeMastro.getVertice(3);

	Ponto pSRU;
	Ponto qSRU;
	Ponto rSRU;
	Ponto sSRU;

	InstanciaPonto(p, pSRU);
	InstanciaPonto(q, qSRU);
	InstanciaPonto(r, rSRU);
	InstanciaPonto(s, sSRU);

	glPopMatrix();

	Poligono oobb;
	oobb.insereVertice(pSRU);
	oobb.insereVertice(qSRU);
	oobb.insereVertice(rSRU);
	oobb.insereVertice(sSRU);

	Ponto min, max;
	oobb.obtemLimites(min, max);

	envelopes[i].alteraVertice(0, Ponto(min.x, min.y));
	envelopes[i].alteraVertice(1, Ponto(min.x, max.y));
	envelopes[i].alteraVertice(2, Ponto(max.x, max.y));
	envelopes[i].alteraVertice(3, Ponto(max.x, min.y));
}

bool CalculaColisao(size_t i) {
	Ponto min, max;
	envelopes[i].obtemLimites(min, max);
	Ponto centroEnvelope((max.x + min.x) / 2, (max.y + min.y) / 2);

	for (size_t j = 0; j < nInstancias; j++) {
		if (i == j || Personagens[j].nroDaCurva != Personagens[i].nroDaCurva) {
			continue;
		}

		Ponto minOutro, maxOutro;
		envelopes[j].obtemLimites(minOutro, maxOutro);

		Ponto centroEnvelopeOutro(
			(maxOutro.x + minOutro.x) / 2, (maxOutro.y + minOutro.y) / 2
		);

		float larg1 = max.x - min.x;
		float alt1 = max.y - min.y;
		float larg2 = maxOutro.x - minOutro.x;
		float alt2 = maxOutro.y - minOutro.y;

		if (
			centroEnvelope.x < centroEnvelopeOutro.x + larg2 &&
			centroEnvelope.x + larg1 > centroEnvelopeOutro.x &&
			centroEnvelope.y < centroEnvelopeOutro.y + alt2 &&
			centroEnvelope.y + alt1 > centroEnvelopeOutro.y)
		{
			return true;
		}
	}

	return false;
}

void MovimentaPersonagens(double tempoDecorrido)
{
	for(size_t i = 0; i < nInstancias; i++) {
		InstanciaBZ *personagem = &Personagens[i];
		personagem->AtualizaPosicao(tempoDecorrido);
		CalculaEnvelope(personagem, i);
		envelopes[i].desenhaPoligono();

		if (i == 0) {
			if (CalculaColisao(i)) {
				exit(0);
			}
		}

		if (personagem->proxCurva != -1) {
			continue;
		}

		if (personagem->tAtual == 0) {
			Bezier *proxCurva = &Curvas[personagem->nroDaCurva];
			personagem->AtualizaIndoParaZ(proxCurva);
			personagem->Curva = &Curvas[personagem->nroDaCurva];
			continue;
		}

		if (personagem->tAtual < 0.5) {
			continue;
		}

		if (personagem->tAtual == 1 && i == 0) {
			curvaFoiSelecionada = false;
			curvaSelecionada = 0;
			continue;
		}

		std::vector<int> curvasPossiveis;

		if (personagem->indoParaZ) {
			for (size_t j = 0; j < nCurvas; j++) {
				if (ligaCurvasZ[personagem->nroDaCurva][j]) {
					curvasPossiveis.push_back(j);
				}
			}
		} else {
			for (size_t j = 0; j < nCurvas; j++) {
				if (ligaCurvasX[personagem->nroDaCurva][j]) {
					curvasPossiveis.push_back(j);
				}
			}
		}

		if (i != 0 || !curvaFoiSelecionada) {
			personagem->proxCurva = curvasPossiveis[
				rand() % curvasPossiveis.size()
			];
		}

		personagem->proxCurva = curvasPossiveis[
			curvaSelecionada % curvasPossiveis.size()
		];
	}
}

// **********************************************************************
//
// **********************************************************************
void DesenhaCurvas()
{
    for(int i=0; i<nCurvas;i++)
    {
        Curvas[i].Traca();
    }
    if(Personagens[0].proxCurva>=0){
        glLineWidth(4.0);
        Curvas[Personagens[0].proxCurva].Traca();
        glLineWidth(0);
    }
}
// **********************************************************************
//  void display( void )
// **********************************************************************
void display( void )
{
	double deltaT = T2.getDeltaT();

	// Limpa a tela coma cor de fundo
	glClear(GL_COLOR_BUFFER_BIT);

    // Define os limites l�gicos da �rea OpenGL dentro da Janela
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// Coloque aqui as chamadas das rotinas que desenham os objetos
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	glLineWidth(1);
	glColor3f(1,1,1); // R, G, B  [0..1]

    DesenhaEixos();

	MovimentaPersonagens(deltaT);
    DesenhaPersonagens(deltaT);
    DesenhaCurvas();

	glutSwapBuffers();
}
// **********************************************************************
// ContaTempo(double tempo)
//      conta um certo n�mero de segundos e informa quanto frames
// se passaram neste per�odo.
// **********************************************************************
void ContaTempo(double tempo)
{
    Temporizador T;

    unsigned long cont = 0;
    cout << "Inicio contagem de " << tempo << "segundos ..." << flush;
    while(true)
    {
        tempo -= T.getDeltaT();
        cont++;
        if (tempo <= 0.0)
        {
            cout << "fim! - Passaram-se " << cont << " frames." << endl;
            break;
        }
    }
}
// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
// **********************************************************************
void keyboard ( unsigned char key, int x, int y )
{

	switch ( key )
	{
		case 27:        // Termina o programa qdo
			exit ( 0 );   // a tecla ESC for pressionada
			break;
        case 't':
            ContaTempo(3);
            break;
        case ' ':
			Personagens[0].parado=!Personagens[0].parado;
            break;
        case 'c':
            Personagens[0].indoParaZ ^= true;
			      Personagens[0].tAtual = 1 - Personagens[0].tAtual;
			      Personagens[0].proxCurva = -1;
            break;
		default:
			break;
	}
}
// **********************************************************************
//  void arrow_keys ( int a_keys, int x, int y )
// **********************************************************************
void arrow_keys ( int a_keys, int x, int y )
{
	switch ( a_keys )
	{
        case GLUT_KEY_LEFT:
			curvaFoiSelecionada = true;
			curvaSelecionada -= 1;
			Personagens[0].proxCurva = -1;
            break;
        case GLUT_KEY_RIGHT:
			curvaFoiSelecionada = true;
			curvaSelecionada += 1;
			Personagens[0].proxCurva = -1;
            break;
		case GLUT_KEY_UP:       // Se pressionar UP
			glutFullScreen ( ); // Vai para Full Screen
			break;
	    case GLUT_KEY_DOWN:     // Se pressionar DOWN
								// Reposiciona a janela
            glutPositionWindow (50,50);
			glutReshapeWindow ( 700, 500 );
			break;
		default:
			break;
	}
}

// **********************************************************************
//  void main ( int argc, char** argv )
//
// **********************************************************************
int  main ( int argc, char** argv )
{
    cout << "Programa OpenGL" << endl;

	srand(time(NULL));

    glutInit            ( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowPosition (0,0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize  ( 650, 500);

    // Cria a janela na tela, definindo o nome da
    // que aparecera na barra de t�tulo da janela.
    glutCreateWindow    ( "Animacao com Bezier" );

    // executa algumas inicializa��es
    init ();

    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // ser� chamada automaticamente quando
    // for necess�rio redesenhar a janela
    glutDisplayFunc ( display );

    // Define que o tratador de evento para
    // o invalida��o da tela. A funcao "display"
    // ser� chamada automaticamente sempre que a
    // m�quina estiver ociosa (idle)
    glutIdleFunc(animate);

    // Define que o tratador de evento para
    // o redimensionamento da janela. A funcao "reshape"
    // ser� chamada automaticamente quando
    // o usu�rio alterar o tamanho da janela
    glutReshapeFunc ( reshape );

    // Define que o tratador de evento para
    // as teclas. A funcao "keyboard"
    // ser� chamada automaticamente sempre
    // o usu�rio pressionar uma tecla comum
    glutKeyboardFunc ( keyboard );

    // Define que o tratador de evento para
    // as teclas especiais(F1, F2,... ALT-A,
    // ALT-B, Teclas de Seta, ...).
    // A funcao "arrow_keys" ser� chamada
    // automaticamente sempre o usu�rio
    // pressionar uma tecla especial
    glutSpecialFunc ( arrow_keys );

    // inicia o tratamento dos eventos
    glutMainLoop ( );

    return 0;
}
