#include <GL/glut.h>
#include <cstdlib>
#include <ctime>
#include<math.h>
#include<iostream>
#include <csignal>
#include <unistd.h>



using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

const int MATRIX_SIZE = 300;

void readMatrixFromFile(const std::string& filename, std::vector<std::vector<float>>& matrix) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return;
    }

    std::string line;
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        if (!std::getline(file, line)) {
            std::cerr << "Error reading line " << i + 1 << std::endl;
            return;
        }

        std::istringstream iss(line);
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            float num;
            if (!(iss >> num)) {
                std::cerr << "Error reading float number at row " << i + 1 << ", column " << j + 1 << std::endl;
                return;
            }
            matrix[i][j] = num;
        }
    }

    file.close();
}

const int WIDTH = 800;
const int HEIGHT = 800;


float rotationX = 0.0f;
float rotationY = 0.0f;
int lastMouseX, lastMouseY;
bool isDragging = false;

int X=-300,Y=-300;

struct ps{
    float pos[2];
};

float heatMatrix[MATRIX_SIZE][MATRIX_SIZE];
struct ps fft(float* r,int* freq,int n,float phi){

   float px,py = 0;
   struct ps poss;
    for (int i = 0; i < n; i++) {
        px += r[i]*cos(freq[i]*phi);
        py += r[i]*sin(freq[i]*phi);
    }
   poss.pos[0] = px;
   poss.pos[1] = py;

cout<<px<<"\n"<<py;
    return poss;
}

void generateHeatMatrix() {
    std::srand(static_cast<unsigned>(std::time(0)));
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            heatMatrix[i][j] = static_cast<float>(std::rand()) / RAND_MAX;
        }
    }
}

void drawHeatMatrix() {
    // glBegin(GL_QUADS);
    // for (int i = 0; i < MATRIX_SIZE; ++i) {
    //     for (int j = 0; j < MATRIX_SIZE; ++j) {
    //         float value = heatMatrix[i][j];
    //         glColor3f(value, 0.0f, 1.0f - value); // Color ranging from blue to red
    //         glVertex3f(i, j, 0.0f);
    //         glVertex3f(i + 1, j, 0.0f);
    //         glVertex3f(i + 1, j + 1, 0.0f);
    //         glVertex3f(i, j + 1, 0.0f);            
    //     }
    // }
    // glEnd();

    float r0=20;
    float r1=4;
    float old_phi = 0;
    std::srand(static_cast<unsigned>(std::time(0)));
    float r[] = {20,4,2,1,.5};
     int freq[] = {1,10,20,30,40};

    std::vector<std::vector<float>> matrix(MATRIX_SIZE, std::vector<float>(MATRIX_SIZE));

    std::string filename = "test.out";
    readMatrixFromFile(filename, matrix);

    // Print the matrix to verify'
    
    for (int i=299;i>=0;i--) {
        for (int j=299;j>=0;j--) {
            glBegin(GL_POINTS);
            glColor3f(matrix[j][i]/255.0, matrix[i][j]/255.0, matrix[i][j]/255.0);
            glPointSize(2);  // wat
            glVertex2i(j-150, i-150);
            glEnd();


            cout<<"a"<<i+j<<"exp("<<matrix[j][i]<<"jt)+";
//            glutSwapBuffers();
//            glutPostRedisplay();
//
//            usleep(1);
        }

    }

//    for(float phi=M_PI/100;phi<2*M_PI;phi+=M_PI/100)
//    {
//        glBegin(GL_LINES);
//        struct ps pos;
//        struct ps old_pos;
//        pos = fft(r,freq,5,phi);
//        //old_pos = fft(r,freq,2,old_phi);
////        glColor3f(1, 0, 0.0f); // Color ranging from blue to red
////        glVertex3f(pos.pos[0], pos.pos[1], 0);
////        glVertex3f(old_pos.pos[0],old_pos.pos[1],0);
//        glEnd();
//        old_phi = phi;
//
//        glBegin(GL_POINTS);
//        glColor3f(.9, 0, 0);
//        glPointSize(2);  // wat
//        glVertex2i(pos.pos[0], pos.pos[1]);
//        glEnd();
//
//        glutSwapBuffers();
//        glutPostRedisplay();
//
//        usleep(100);
//    }


    // glBegin(GL_LINES);
    //     glVertex3f(0.0f, 0.0f, 0.0f);
    //     glVertex3f(-50.0f, 50.0f, 50.0f);
    // glEnd();

    // glBegin(GL_LINES);
    //     glVertex3f(0.0f, 0.0f, 0.0f);
    //     glVertex3f(50.0f, -50.0f, 50.0f);
    // glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(MATRIX_SIZE / 2.0f, MATRIX_SIZE / 2.0f, -200.0f);
    glRotatef(rotationX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotationY, 0.0f, 1.0f, 0.0f);
    glTranslatef(-MATRIX_SIZE / 2.0f, -MATRIX_SIZE / 2.0f, 0.0f);

    drawHeatMatrix();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isDragging = true;
            lastMouseX = x;
            lastMouseY = y;
        } else {
            isDragging = false;
        }
    }
}

void mouseMotion(int x, int y) {
    if (isDragging) {
        rotationX += (y - lastMouseY);
        rotationY += (x - lastMouseX);
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
   
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("DDPU");

    glEnable(GL_DEPTH_TEST);

    generateHeatMatrix();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);

    glutMainLoop();
    return 0;
}
