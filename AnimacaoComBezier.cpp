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

Bezier Curvas[26];
unsigned int nCurvas;

// Limites l�gicos da �rea de desenho
Ponto Min, Max;

//lista de curvas que tem interseccao
bool ligaCurvasX[26][26];
bool ligaCurvasZ[26][26];

bool desenha = false, mover=false;

Poligono Mapa, MeiaSeta, Mastro, CurvasBZ, PontosCurvas;
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
void DesenhaPlayer()
{
    defineCor(LimeGreen);
    Mastro.desenhaPoligono();
}

void DesenhaInimigo()
{
    defineCor(rand() % 100);
    Mastro.desenhaPoligono();
}
// **********************************************************************
// Esta fun��o deve instanciar todos os personagens do cen�rio
// **********************************************************************
void CriaInstancias()
{
    Personagens[0].Posicao = Ponto (3,0);
    Personagens[0].modelo = DesenhaPlayer;
    Personagens[0].indoParaZ = true;
    Personagens[0].Escala = Ponto (0.4,0.4,0.4);
	Personagens[0].Curva = &Curvas[0];
    nInstancias = 11;

	for (size_t i = 1; i < nInstancias; i++) {
		Personagens[i].Posicao = Ponto (0,0);
		Personagens[i].modelo = DesenhaInimigo;
		Personagens[i].indoParaZ = true;
		Personagens[i].Escala = Ponto (0.4,0.4,0.4);
		Personagens[i].Curva = &Curvas[i];
	}


}
// **********************************************************************
//
// **********************************************************************
void CarregaModelos()
{
    Mapa.LePoligono("EstadoRS.txt");
    MeiaSeta.LePoligono("MeiaSeta.txt");
    Mastro.LePoligono("Triangulo.txt");
    PontosCurvas.LePoligono("cordPontos.txt");
    CurvasBZ.LePoligonoZ("ListaCurvas.txt");
}
void CriaCurvas()
{
    nCurvas = CurvasBZ.getNVertices(); //qtd curvas no topo do txt de curvas
    for(int i =0;i<nCurvas;i++){
        Curvas[i] = Bezier(PontosCurvas.getVertice(CurvasBZ.getVertice(i).x), PontosCurvas.getVertice(CurvasBZ.getVertice(i).y), PontosCurvas.getVertice(CurvasBZ.getVertice(i).z));
    }
}

//cria matriz com as curvas que se encontram
void encontroCurvas(){
    for(int i=0; i<26;i++){
        for(int j=0; j<26;j++){
            if(i==j){
                ligaCurvasX[i][j]=false;
                ligaCurvasZ[i][j]=false;
            }
            else{
                if(CurvasBZ.getVertice(i).x==CurvasBZ.getVertice(j).x
                ||CurvasBZ.getVertice(i).x==CurvasBZ.getVertice(j).z){
                    ligaCurvasX[i][j]=true;
                }
                else{
                    ligaCurvasX[i][j]=false;
                }
                if(CurvasBZ.getVertice(i).z==CurvasBZ.getVertice(j).x
                ||CurvasBZ.getVertice(i).z==CurvasBZ.getVertice(j).z){
                    ligaCurvasZ[i][j]=true;
                }
                else{
                    ligaCurvasZ[i][j]=false;
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
    cout << "nInstancias: "<< nInstancias << endl;
    for(int i=0; i<nInstancias;i++)
    {
        Personagens[i].AtualizaPosicao(tempoDecorrido);

		if (Personagens[i].proxCurva<0) {
			if (Personagens[i].tAtual >= 0.5) {
                //lista das curvas possíveis
				std::vector<int> curvasPossiveis;

				if (Personagens[i].indoParaZ) {
					for (size_t j = 0; j < nCurvas; j++) {
						if (ligaCurvasZ[Personagens[i].nroDaCurva][j]) {
							curvasPossiveis.push_back(j);
						}
					}
				} else {
					for (size_t j = 0; j < nCurvas; j++) {
						if (ligaCurvasX[Personagens[i].nroDaCurva][j]) {
							curvasPossiveis.push_back(j);
						}
					}
				}
                Personagens[i].curvaListaCurvas=rand() % curvasPossiveis.size();
				Personagens[i].proxCurva = curvasPossiveis[Personagens[i].curvaListaCurvas];
                if(!Personagens[i].listaCurvasPos){
                    Personagens[i].curvasLigadas =curvasPossiveis;
                    Personagens[i].listaCurvasPos=true;
                }
			} else if (Personagens[i].tAtual == 0) {
				Personagens[i].Curva = &Curvas[Personagens[i].nroDaCurva];
                std::vector<int> vazio;
                Personagens[i].curvasLigadas = vazio;
                Personagens[i].listaCurvasPos=false;
                Personagens[i].proxCurva=-1;
			}
		}

        Personagens[i].desenha();
    }
}
// **********************************************************************
//
// **********************************************************************
void DesenhaCurvas()
{
    for(int i=0; i<nCurvas;i++)
    {
        defineCor(Gold);
        Curvas[i].Traca();
    }
}
// **********************************************************************
//  void display( void )
// **********************************************************************
void display( void )
{

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

    DesenhaPersonagens(T2.getDeltaT());
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
			if (Personagens[0].Velocidade == 0) {
				if (Personagens[0].direcao == 1) {
					Personagens[0].Velocidade = 1.0;
				} else {
					Personagens[0].Velocidade = -1.0;
				}
			} else {
				Personagens[0].Velocidade = 0;
			}
            break;
        case 'c':
            Personagens[0].Rotacao-=180;
            Personagens[0].indoParaZ=!Personagens[0].indoParaZ;
            if (Personagens[0].tAtual >= 0.5) {
                //lista das curvas possíveis
				std::vector<int> curvasPossiveis;

				if (Personagens[0].indoParaZ) {
					for (size_t j = 0; j < nCurvas; j++) {
						if (ligaCurvasZ[Personagens[0].nroDaCurva][j]) {
							curvasPossiveis.push_back(j);
						}
					}
				} else {
					for (size_t j = 0; j < nCurvas; j++) {
						if (ligaCurvasX[Personagens[0].nroDaCurva][j]) {
							curvasPossiveis.push_back(j);
						}
					}
				}
                Personagens[0].curvaListaCurvas=rand() % curvasPossiveis.size();
				Personagens[0].proxCurva = curvasPossiveis[Personagens[0].curvaListaCurvas];
                if(!Personagens[0].listaCurvasPos){
                    Personagens[0].curvasLigadas =curvasPossiveis;
                    Personagens[0].listaCurvasPos=true;
                }
			}
            if(Personagens[0].direcao==1){
                Personagens[0].direcao=0;

				if (Personagens[0].Velocidade != 0) {
					Personagens[0].Velocidade = -1.0;
				}
            }
            else{
                Personagens[0].direcao=1;

				if (Personagens[0].Velocidade != 0) {
					Personagens[0].Velocidade = 1.0;
				}
            }
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
            //diminuir 1 na proxima curva
            if(Personagens[0].listaCurvasPos){
                if(Personagens[0].curvaListaCurvas+1==Personagens[0].curvasLigadas.size()){
                    Personagens[0].proxCurva=Personagens[0].curvasLigadas[0];
                }
                else{
                    Personagens[0].curvaListaCurvas++;
                    Personagens[0].proxCurva=Personagens[0].curvasLigadas[Personagens[0].curvaListaCurvas];
                    }
                    //ver como pintar proxima curva
                    defineCor(Firebrick);
                    Curvas[Personagens[0].proxCurva].Traca();
            }
            break;
        case GLUT_KEY_RIGHT:
            //aumentar 1 na proxima curva
            if(Personagens[0].listaCurvasPos){
                if(Personagens[0].curvaListaCurvas==0){
                    Personagens[0].proxCurva=Personagens[0].curvasLigadas[Personagens[0].curvasLigadas.size()-1];
                }
                else{
                    Personagens[0].curvaListaCurvas--;
                    Personagens[0].proxCurva=Personagens[0].curvasLigadas[Personagens[0].curvaListaCurvas];
                    }
                //ver como pintar proxima curva
                defineCor(Firebrick);
                Curvas[Personagens[0].proxCurva].Traca();
            }
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
