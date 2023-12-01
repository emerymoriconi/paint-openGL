/*
 * Computacao Grafica
 * Codigo Exemplo: Biblioteca para escrever textos na janela GLUT/OpenGL
 * Autor: Prof. Laurindo de Sousa Britto Neto
 */

#ifndef glut_text_h
#define glut_text_h

#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include <string>
#include <sstream>

/*
 * Parametros padroes
 */
#define SCALE 0.0007
#define LINE_WIDTH 1.0
GLvoid * font_bitmap = GLUT_BITMAP_HELVETICA_18;
GLvoid * font_stroke = GLUT_STROKE_ROMAN;

//cria o namespace my com a declaracao da assinatura da fucao to_string
namespace my
{
    template<typename T> std::string to_string(const T& value);
}

#if (__cplusplus >= 201103L) // Verifica a versao do compilador C++
  using std::to_string; //usada a to_string do namespace std se a versao for maior ou igual ao C++11
#else
  using my::to_string;  //usada a to_string do namespace my em caso contrario
#endif

/*
 * Converte numeros em std::string
 */
template<typename T> std::string my::to_string(const T& value)
{
    std::ostringstream s;
    s << value;
    return s.str();
}

/*
 * Desenha texto bitmap na posicao (x,y)
 */
void draw_text_bitmap(float x, float y, std::string text, void * font = font_bitmap)
{
    glRasterPos2f(x, y);
    for(char * i = (char *)text.c_str(); *i; i++){ //for(char i : text){ //C++11
        glutBitmapCharacter(font, *i); //glutBitmapCharacter(font, i); //C++11
    }
}

/*
 * Desenha texto stroke na posicao (x,y)
 */
void draw_text_stroke(float x, float y, std::string text, float scale = SCALE, float lineWidth = LINE_WIDTH, void * font = font_stroke)
{
    glPushMatrix();
        glTranslatef(x, y, 0);
        glScalef(scale, scale, scale); // define o tamanho do fonte
        glLineWidth(lineWidth); // define a espessura da fonte
        for(char * i = (char *)text.c_str(); *i; i++){
            glutStrokeCharacter(font, *i);
        }
    glPopMatrix();
}

#endif /* glut_text_h */
