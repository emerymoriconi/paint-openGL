/*
 * Computacao Grafica
 * Codigo Exemplo: Rasterizacao de Segmentos de Reta com GLUT/OpenGL
 * Autor: Émery Freitas Moriconi
   Créditos: Prof. Laurindo de Sousa Britto Neto
 */

// Bibliotecas utilizadas pelo OpenGL
#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
    #include <GLUT/glut.h>
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/glut.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <forward_list>
#include "glut_text.h"

using namespace std;

// Variaveis Globais
#define ESC 27

//Enumeracao com os tipos de formas geometricas
enum tipo_forma{LIN = 1, RET = 2, TRI, POL, CIR }; // Linha, Triangulo, Retangulo, Poligono, Circulo

//Verifica se foi realizado o primeiro clique do mouse
bool click1 = false;

//Coordenadas da posicao atual do mouse
int m_x, m_y;

//Coordenadas do primeiro clique e do segundo clique do mouse
int x_1, y_1, x_2, y_2;

//Indica o tipo de forma geometrica ativa para desenhar
int modo = LIN;

//Largura e altura da janela
int width = 512, height = 512;

// Definicao de vertice
struct vertice{
    int x;
    int y;
};

// Definicao das formas geometricas
struct forma{
    int tipo;
    forward_list<vertice> v; //lista encadeada de vertices
};

// Lista encadeada de formas geometricas
forward_list<forma> formas;

// Funcao para armazenar uma forma geometrica na lista de formas
// Armazena sempre no inicio da lista
void pushForma(int tipo){
    forma f;
    f.tipo = tipo;
    formas.push_front(f);
}

// Funcao para armazenar um vertice na forma do inicio da lista de formas geometricas
// Armazena sempre no inicio da lista
void pushVertice(int x, int y){
    vertice v;
    v.x = x;
    v.y = y;
    formas.front().v.push_front(v);
}

//Fucao para armazenar uma Linha na lista de formas geometricas
void pushLinha(int x1, int y1, int x2, int y2){
    pushForma(LIN);
    pushVertice(x1, y1);
    pushVertice(x2, y2);
}

/*
 * Declaracoes antecipadas (forward) das funcoes (assinaturas das funcoes)
 */
void init(void);
void reshape(int w, int h);
void display(void);
void menu_popup(int value);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void mousePassiveMotion(int x, int y);
void drawPixel(int x, int y);
// Funcao que percorre a lista de formas geometricas, desenhando-as na tela
void drawFormas();
// Funcao que implementa o Algoritmo de Bresenham para rasterizacao de segmentos de retas
void retaBresenham(int x1,int y1,int x2,int y2);
void quadrilatero(int x1, int y1, int x2, int y2);


/*
 * Funcao principal
 */
int main(int argc, char** argv){
    glutInit(&argc, argv); // Passagens de parametro C para o glut
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); //Selecao do Modo do Display e do Sistema de cor
    glutInitWindowSize (width, height);  // Tamanho da janela do OpenGL
    glutInitWindowPosition (100, 100); //Posicao inicial da janela do OpenGL
    glutCreateWindow ("Computacao Grafica: Bresenham"); // Da nome para uma janela OpenGL
    init(); // Chama funcao init();
    glutReshapeFunc(reshape); //funcao callback para redesenhar a tela
    glutKeyboardFunc(keyboard); //funcao callback do teclado
    glutMouseFunc(mouse); //funcao callback do mouse
    glutPassiveMotionFunc(mousePassiveMotion); //fucao callback do movimento passivo do mouse
    glutDisplayFunc(display); //funcao callback de desenho
    
    // Define o menu pop-up
    glutCreateMenu(menu_popup);
    glutAddMenuEntry("Linha", LIN);
    glutAddMenuEntry("Retangulo", RET);
    glutAddMenuEntry("Sair", 0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    
    glutMainLoop(); // executa o loop do OpenGL
    return EXIT_SUCCESS; // retorna 0 para o tipo inteiro da funcao main();
}

/*
 * Inicializa alguns parametros do GLUT
 */
void init(void){
    glClearColor(1.0, 1.0, 1.0, 1.0); //Limpa a tela com a cor branca;
}

/*
 * Ajusta a projecao para o redesenho da janela
 */
void reshape(int w, int h)
{
	// Muda para o modo de projecao e reinicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Definindo o Viewport para o tamanho da janela
	glViewport(0, 0, w, h);
	
	width = w;
	height = h;
    glOrtho (0, w, 0, h, -1 ,1);  

   // muda para o modo de desenho
	glMatrixMode(GL_MODELVIEW);
 	glLoadIdentity();

}

/*
 * Controla os desenhos na tela
 */
void display(void){
    glClear(GL_COLOR_BUFFER_BIT); //Limpa o buffer de cores e reinicia a matriz
    glColor3f (0.0, 0.0, 0.0); // Seleciona a cor default como preto
    drawFormas(); // Desenha as formas geometricas da lista
    //Desenha texto com as coordenadas da posicao do mouse
    draw_text_stroke(0, 0, "(" + to_string(m_x) + "," + to_string(m_y) + ")", 0.2);
    glutSwapBuffers(); // manda o OpenGl renderizar as primitivas
}

/*
 * Controla o menu pop-up
 */
void menu_popup(int value){
    if (value == 0) exit(EXIT_SUCCESS);
    modo = value;
}


/*
 * Controle das teclas comuns do teclado
 */
void keyboard(unsigned char key, int x, int y){
    switch (key) { // key - variavel que possui valor ASCII da tecla precionada
        case ESC: exit(EXIT_SUCCESS); break;
    }
}

/*
 * Controle dos botoes do mouse
 */
void mouse(int button, int state, int x, int y){
    switch (button) {
        case GLUT_LEFT_BUTTON:
            switch(modo){
                case LIN:
                    if (state == GLUT_DOWN) {
                        if(click1){
                            x_2 = x;
                            y_2 = height - y - 1;
                            printf("Clique 2(%d, %d)\n",x_2,y_2);
                            pushLinha(x_1, y_1, x_2, y_2);
                            click1 = false;
                            glutPostRedisplay();
                        }else{
                            click1 = true;
                            x_1 = x;
                            y_1 = height - y - 1;
                            printf("Clique 1(%d, %d)\n",x_1,y_1);
                        }
                    }
                break;
            	
            	case RET:
                    if (state == GLUT_DOWN) {
                        if (click1) {
                            x_2 = x;
                            y_2 = height - y - 1;
                            printf("Clique 2(%d, %d)\n",x_2,y_2);
                            quadrilatero(x_1, y_1, x_2, y_2);  // Usando a função quadrilatero para desenhar
                            click1 = false;
                            glutPostRedisplay();
                        } else {
                            click1 = true;
                            x_1 = x;
                            y_1 = height - y - 1;
                            printf("Clique 1(%d, %d)\n",x_1,y_1);
                        }
                    }
                break;
            }
        break;
    }
}
/*
 * Controle da posicao do cursor do mouse
 */
void mousePassiveMotion(int x, int y){
    m_x = x; m_y = height - y - 1;
    glutPostRedisplay();
}

/*
 * Funcao para desenhar apenas um pixel na tela
 */
void drawPixel(int x, int y){
    glBegin(GL_POINTS); // Seleciona a primitiva GL_POINTS para desenhar
        glVertex2i(x, y);
    glEnd();  // indica o fim do ponto
}

/*
 *Funcao que desenha a lista de formas geometricas
 */
void drawFormas() {
	// Apos o primeiro clique, desenha a reta com a posicao atual do mouse
	if (click1) retaBresenham(x_1, y_1, m_x, m_y);
    // Percorre a lista de formas geometricas para desenhar
    // Percorre a lista de formas geometricas para desenhar
    for (auto f = formas.begin(); f != formas.end(); ++f) {
        switch (f->tipo) {
            case LIN: {
                // Percorre a lista de vertices da forma linha para desenhar
                auto it = f->v.begin();
                auto end = f->v.end();
                while (it != end) {
                    auto next = std::next(it);
                    if (next == end) {
                        next = f->v.begin();  // Conecta o último vértice ao primeiro
                    }
                    retaBresenham(it->x, it->y, next->x, next->y);
                    ++it;
                }
                break;
            }
            case RET: {
                // Percorre a lista de vertices da forma retangulo para desenhar
                auto it = f->v.begin();
                auto end = f->v.end();
                while (it != end) {
                    auto next = std::next(it);
                    if (next == end) {
                        next = f->v.begin();  // Conecta o último vértice ao primeiro
                    }
                    retaBresenham(it->x, it->y, next->x, next->y);
                    ++it;
                }
 	  	 	 	break;
            }
        }
    }
}

/*
 * Funcao que implementa o Algoritmo de Rasterizacao de Bresenham 
 */
void retaBresenham(int x1, int y1, int x2, int y2){ 
	bool declive=false, simetrico=false;
    int deltaX, deltaY, d, incE, incNE;
        
	deltaX = (x2 - x1);
    deltaY = (y2 - y1);
    
    //redução ao primeiro octante
    if(((deltaX)*(deltaY))<0){
		y1 = y1 * (-1);
		y2 = y2 * (-1);
		deltaY = deltaY * (-1);
		simetrico = true;
	}
	
	if(abs(deltaX)<abs(deltaY)){
		swap(x1, y1); 
		swap(x2, y2);
		swap(deltaX, deltaY);
		declive = true;
	}
	
	if(x1>x2){
		swap(x1, x2);
		swap(y1, y2);
		
		deltaX = deltaX * (-1);
		deltaY = deltaY * (-1);
	}
	   
	//Bresenham	
	d = (2 * deltaY) - deltaX;
    incE = 2 * deltaY;
    incNE = 2 * (deltaY - deltaX); 
	
    for(int x = (int)x1; x <= x2; x++){
    	int x_ = x, y_ = y1;
		if(d <= 0){
			d = d + incE;
		} else {
			y1 = y1 + 1;
			d = d + incNE;
		}
			
		if((declive)==true){
			swap(x_, y_);
		}	
		
		if((simetrico)==true){
			y_ = y_ * (-1);
		}
		drawPixel(x_,(int)y_);
    }
}

void quadrilatero(int x1, int y1, int x2, int y2){
	// Adiciona a forma retângulo à lista de formas
    pushForma(RET);
    
    // Adiciona os quatro vértices do retângulo à lista de vértices
    pushVertice(x1, y1);
    pushVertice(x2, y1);
    pushVertice(x2, y2);
    pushVertice(x1, y2);
}