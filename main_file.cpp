/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE
#define _GLIBCXX_USE_CXX11_ABI 0
#define _GLIBCXX_USE_C99 1

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext.hpp"
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <utility>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "vicviper12.h"
#include "SwordfishII.h"
#include "myCube.h"

float speed_x=0;
float speed_y=0;
float speed_bullet=3;
float speed_enemy=0.5;
float shotCooldown = 0;
bool shot = false;
bool gameOver = false;
float aspectRatio=1;
ShaderProgram *sp;
std::vector< std::pair<float,float> > bulletPos;

//Uchwyty na tekstury
GLuint tex0;
GLuint tex1;
GLuint tex2;



class Model{
    private:
        glm::mat4 M = glm::mat4(1.0);
    public:
        unsigned int vertexCount;
        float x,z;
        float* verts;
        float* normals;
        float* texCoords;

    void rot(float angle, glm::vec3  ax){
        this->M = glm::rotate(this->M, angle, ax);
    }

    void translate(glm::vec3 v){
        this->M=glm::translate(this->M, v);
    }

    void scale(glm::vec3 v){
        this->M=glm::scale(this->M, v);
    }

    Model(){};

    Model(float x, float z, float* verts, float* normals, float* texCoords, unsigned int vertexCount){
        this->x = x;
        this->z = z;
        this->verts = verts;
        this->normals = normals;
        this->texCoords = texCoords;
        this->vertexCount = vertexCount;
    }

    void draw(glm::mat4 P, glm::mat4 V, ShaderProgram *shader, GLFWwindow* window){

        //Przeslij parametry programu cieniującego do karty graficznej
        glUniformMatrix4fv(shader->u("P"),1,false,glm::value_ptr(P));
        glUniformMatrix4fv(shader->u("V"),1,false,glm::value_ptr(V));
        glUniformMatrix4fv(shader->u("M"),1,false,glm::value_ptr(this->M));

        glEnableVertexAttribArray(shader->a("vertex"));  //Włącz przesyłanie danych do atrybutu vertex
        glVertexAttribPointer(shader->a("vertex"),3,GL_FLOAT,false,0,this->verts); //Wskaż tablicę z danymi dla atrybutu vertex

        glEnableVertexAttribArray(shader->a("normal"));  //Włącz przesyłanie danych do atrybutu normal
        glVertexAttribPointer(shader->a("normal"),3,GL_FLOAT,false,0,this->normals); //Wskaż tablicę z danymi dla atrybutu normal


        glDrawArrays(GL_TRIANGLES,0,this->vertexCount); //Narysuj obiekt

        glDisableVertexAttribArray(sp->a("vertex"));  //Wyłącz przesyłanie danych do atrybutu vertex
        glDisableVertexAttribArray(sp->a("normal"));  //Wyłącz przesyłanie danych do atrybutu normal
        glDisableVertexAttribArray(sp->a("texCoord0"));  //Wyłącz przesyłanie danych do atrybutu texCoord0

    }

};

std::vector<Model> enemies;

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods) {
    if (action==GLFW_PRESS) {
        if (key==GLFW_KEY_LEFT) speed_x=1;
        if (key==GLFW_KEY_RIGHT) speed_x=-1;
        if (key==GLFW_KEY_UP) speed_y=1;
        if (key==GLFW_KEY_DOWN) speed_y=-1;
        if (key==GLFW_KEY_SPACE) shot = true;
    }
    if (action==GLFW_RELEASE) {
        if (key==GLFW_KEY_LEFT) speed_x=0;
        if (key==GLFW_KEY_RIGHT) speed_x=0;
        if (key==GLFW_KEY_UP) speed_y=0;
        if (key==GLFW_KEY_DOWN) speed_y=0;
    }
}

void windowResizeCallback(GLFWwindow* window,int width,int height) {
    if (height==0) return;
    aspectRatio=(float)width/(float)height;
    glViewport(0,0,width,height);
}



//Funkcja wczytująca teksturę
GLuint readTexture(const char* filename) {
  GLuint tex;
  glActiveTexture(GL_TEXTURE0);

  //Wczytanie do pamięci komputera
  std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
  unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
  //Wczytaj obrazek
  unsigned error = lodepng::decode(image, width, height, filename);

  //Import do pamięci karty graficznej
  glGenTextures(1,&tex); //Zainicjuj jeden uchwyt
  glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
  //Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
  glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image.data());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return tex;
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);

	sp=new ShaderProgram("vertex.glsl",NULL,"fragment.glsl");

    tex0=readTexture("skin.png");
    tex1=readTexture("Swordfish_II.png");
    tex2=readTexture("metal.png");
}

bool checkCollision(Model a, Model b){
    if(glm::sqrt(glm::pow(a.x-b.x,2)+glm::pow(a.z-b.z,2)) < 1){
        return true;
    }
}

//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************
    glDeleteTextures(1,&tex0);
    glDeleteTextures(1,&tex1);
    delete sp;
}


//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window,float mov_x,float mov_z,std::vector< std::pair<float,float> > *bulletPos, bool shot) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V=glm::lookAt(
         glm::vec3(0, 5, -10),
         glm::vec3(0,2,0),
         glm::vec3(0.0f,1.0f,0.0f)); //Wylicz macierz widoku

    glm::mat4 P=glm::perspective(50.0f*PI/180.0f, aspectRatio, 0.01f, 50.0f); //Wylicz macierz rzutowania


	//Model
    Model ship;
	if(!gameOver){
        ship.x = mov_x;
        ship.z = mov_z;
        ship.verts=viperVertices;
        ship.normals=viperNormals;
        ship.texCoords=viperTexCoords;
        ship.vertexCount=viperVertexCount;
        ship.translate(glm::vec3(ship.x, 0.0f, 0.0f));
        ship.translate(glm::vec3(0.0f, 0.0f, ship.z));
        ship.scale(glm::vec3(0.01f,0.01f,0.01f));

        sp->use();//Aktywacja programu cieniującego

        glEnableVertexAttribArray(sp->a("texCoord0"));  //Włącz przesyłanie danych do atrybutu texCoord0
        glVertexAttribPointer(sp->a("texCoord0"),2,GL_FLOAT,false,0,ship.texCoords); //Wskaż tablicę z danymi dla atrybutu texCoord0
        glUniform1i(sp->u("textureMap0"),0);

        glUniform4f(sp->u("lp"),ship.x,0,ship.z+1,1); //Współrzędne źródła światła

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,tex0);

        ship.draw(P, V, sp, window);
    }

    for(auto enemy : enemies){
        enemy.translate(glm::vec3(enemy.x, 0.0f, 0.0f));
        enemy.translate(glm::vec3(0.0f, 0.0f, enemy.z));
        enemy.rot(PI, glm::vec3(0.0f,1.0f,0.0f));
        enemy.scale(glm::vec3(0.1f, 0.1f, 0.1f));

        sp->use();//Aktywacja programu cieniującego

        glEnableVertexAttribArray(sp->a("texCoord0"));  //Włącz przesyłanie danych do atrybutu texCoord0
        glVertexAttribPointer(sp->a("texCoord0"),2,GL_FLOAT,false,0,enemy.texCoords); //Wskaż tablicę z danymi dla atrybutu texCoord0
        glUniform1i(sp->u("textureMap0"),0);

        glUniform4f(sp->u("lp"),enemy.x,0,enemy.z+3,1); //Współrzędne źródła światła

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,tex1);

        enemy.draw(P, V, sp, window);

        if(checkCollision(enemy, ship)){
            gameOver = true;
        }
    }

//    Model enemy;
//	enemy.x = 4;
//	enemy.z = 15;
//	enemy.verts=swordfishVertices;
//	enemy.normals=swordfishNormals;
//	enemy.texCoords=swordfishTexCoords;
//	enemy.vertexCount=swordfishVertexCount;
//    enemy.translate(glm::vec3(enemy.x, 0.0f, 0.0f));
//    enemy.translate(glm::vec3(0.0f, 0.0f, enemy.z));
//    enemy.rot(PI, glm::vec3(0.0f,1.0f,0.0f));
//    enemy.scale(glm::vec3(0.2f, 0.2f, 0.2f));


//    std::cout << mov_bullet <<  " ";
    for(auto pos : *bulletPos){
        Model bullet(pos.first, pos.second, myCubeVertices, myCubeNormals, myCubeTexCoords, myCubeVertexCount);
        bullet.translate(glm::vec3(bullet.x, 0.0f, 0.0f));
        bullet.translate(glm::vec3(0.0f, 0.0f, bullet.z));
        bullet.rot(bullet.z/bullet.x, glm::vec3(0.0f,0.0f,1.0f));
        bullet.scale(glm::vec3(0.05f, 0.05f, 0.05f));

        sp->use();//Aktywacja programu cieniującego

        glEnableVertexAttribArray(sp->a("texCoord0"));  //Włącz przesyłanie danych do atrybutu texCoord0
        glVertexAttribPointer(sp->a("texCoord0"),2,GL_FLOAT,false,0,bullet.texCoords); //Wskaż tablicę z danymi dla atrybutu texCoord0
        glUniform1i(sp->u("textureMap0"),0);

//        glUniform4f(sp->u("lp"),pos.first,0, pos.second+0.5,1); //Współrzędne źródła światła

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,tex2);

        bullet.draw(P, V, sp, window);
        for(auto it = enemies.begin(); it != enemies.end(); it++){
            if(checkCollision(bullet, *it)){
                it = enemies.erase(it);
            }
        }
    }
    if(shot == true && shotCooldown <= 0)
        bulletPos->emplace_back(ship.x, ship.z);

    glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni

}


int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(1024, 1024, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	//Główna pętla
	float mov_x=0; //Aktualny kąt obrotu obiektu
	float mov_z=0; //Aktualny kąt obrotu obiektu
	for(int i = 0; i < 20; i++){
        enemies.emplace_back((i%5)*1.5-3, i/5*3+7, swordfishVertices, swordfishNormals, swordfishTexCoords, swordfishVertexCount);
	}
    std::vector< std::pair<float,float> > bulletPos;
	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
	    if(glm::abs(mov_x+speed_x*glfwGetTime()) < 4)
            mov_x+=speed_x*glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
        if(glm::abs(mov_z+speed_y*glfwGetTime()) < 3)
            mov_z+=speed_y*glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
        for(int i = 0; i < bulletPos.size(); i++){
            bulletPos[i].second = bulletPos[i].second + speed_bullet*glfwGetTime();
            if(bulletPos[i].second > 50)
                bulletPos.erase(bulletPos.begin()+i);
        }
        if(shotCooldown > 0)
            shotCooldown -= glfwGetTime();
        for(int i = 0; i < enemies.size(); i++){
            if((*enemies.begin()).z > 1)
                enemies[i].z -= glfwGetTime()*speed_enemy;
        }
        glfwSetTime(0); //Zeruj timer
		drawScene(window,mov_x,mov_z,&bulletPos,shot); //Wykonaj procedurę rysującą
		if(shot == true && shotCooldown <= 0){
            shotCooldown = 0.5;
            shot = false;
		}
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
