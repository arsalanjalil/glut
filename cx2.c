#include <string.h>
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/* Surface function z = f(x,y) = x^2 + y^2 */
static inline float f(float x, float y) { return x*x + y*y; }
/* Partial derivatives: fx = 2x, fy = 2y */
static inline void normal_from_xy(float x, float y, float n[3]) {
    /* For a graph z = f(x,y), a normal vector is (-fx, -fy, 1) */
    float nx = -2.0f * x;
    float ny = -2.0f * y;
    float nz = 1.0f;
    float len = sqrtf(nx*nx + ny*ny + nz*nz);
    if (len == 0.0f) len = 1.0f;
    n[0] = nx/len; n[1] = ny/len; n[2] = nz/len;
}

/* View and render parameters */
int window_width = 800, window_height = 600;
float rotX = 25.0f, rotY = -30.0f; /* degrees */
float zoom = 1.0f; /* scale factor */
int lastMouseX = -1, lastMouseY = -1;
int leftButtonDown = 0;
int wireframe = 0;

/* Surface grid */
int slices = 120; /* subdivisions in x */
int stacks = 120; /* subdivisions in y */
float range = 1.5f; /* draw x,y in [-range, +range] */

/* Light/material setup */
void setup_lighting(void) {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat light_pos[] = {2.0f, 2.0f, 5.0f, 1.0f};
    GLfloat ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat spec[] = {0.9f, 0.9f, 0.9f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

    GLfloat mat_ambient[] = {0.2f, 0.3f, 0.6f, 1.0f};
    GLfloat mat_diffuse[] = {0.2f, 0.4f, 0.8f, 1.0f};
    GLfloat mat_specular[] = {0.9f, 0.9f, 0.9f, 1.0f};
    GLfloat mat_shininess[] = {50.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

void draw_axes(float len) {
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
      glColor3f(1,0,0); glVertex3f(0,0,0); glVertex3f(len,0,0);
      glColor3f(0,1,0); glVertex3f(0,0,0); glVertex3f(0,len,0);
      glColor3f(0,0,1); glVertex3f(0,0,0); glVertex3f(0,0,len);
    glEnd();
    glEnable(GL_LIGHTING);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* place camera a bit back along Z */
    glTranslatef(0.0f, 0.0f, -4.0f * zoom);
    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    setup_lighting();

    if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_POLYGON_OFFSET_FILL);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);
    }

    /* Draw surface as a grid of quads */
    float dx = (2.0f * range) / (float)slices;
    float dy = (2.0f * range) / (float)stacks;

    for (int i = 0; i < slices; ++i) {
        float x0 = -range + i * dx;
        float x1 = x0 + dx;
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= stacks; ++j) {
            float y = -range + j * dy;
            float z1 = f(x1, y);
            float z0 = f(x0, y);

            float n1[3], n0[3];
            normal_from_xy(x1, y, n1);
            normal_from_xy(x0, y, n0);

            glNormal3fv(n1);
            glVertex3f(x1, y, z1);

            glNormal3fv(n0);
            glVertex3f(x0, y, z0);
        }
        glEnd();
    }

    /* optional axes */
    draw_axes(1.0f);

    glutSwapBuffers();
}

void reshape(int w, int h) {
    window_width = w; window_height = h;
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (h == 0) ? 1.0f : (float)w / (float)h;
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;
    switch (key) {
        case 'q': case 'Q': case 27: /* ESC */
            exit(0);
            break;
        case 'w': case 'W':
            wireframe = !wireframe;
            glutPostRedisplay();
            break;
        case '+':
            zoom *= 0.9f; if (zoom < 0.1f) zoom = 0.1f;
            glutPostRedisplay();
            break;
        case '-':
            zoom *= 1.1f; if (zoom > 10.0f) zoom = 10.0f;
            glutPostRedisplay();
            break;
        case 'r': case 'R':
            rotX = 25.0f; rotY = -30.0f; zoom = 1.0f; glutPostRedisplay();
            break;
    }
}

void special_keys(int key, int x, int y) {
    (void)x; (void)y;
    const float d = 5.0f;
    switch (key) {
        case GLUT_KEY_LEFT: rotY -= d; break;
        case GLUT_KEY_RIGHT: rotY += d; break;
        case GLUT_KEY_UP: rotX -= d; break;
        case GLUT_KEY_DOWN: rotX += d; break;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        leftButtonDown = (state == GLUT_DOWN);
        lastMouseX = x; lastMouseY = y;
    }
    /* Wheel handling (GLUT wheel uses button 3/4 in some implementations) */
    if (button == 3) { /* wheel up */ zoom *= 0.9f; if (zoom < 0.1f) zoom = 0.1f; glutPostRedisplay(); }
    if (button == 4) { /* wheel down */ zoom *= 1.1f; if (zoom > 10.0f) zoom = 10.0f; glutPostRedisplay(); }
}

void motion(int x, int y) {
    if (!leftButtonDown) return;
    int dx = x - lastMouseX;
    int dy = y - lastMouseY;
    rotY += dx * 0.5f;
    rotX += dy * 0.5f;
    lastMouseX = x; lastMouseY = y;
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    /* Optional command-line overrides */
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-slices") == 0 && i+1 < argc) { slices = atoi(argv[++i]); }
        else if (strcmp(argv[i], "-stacks") == 0 && i+1 < argc) { stacks = atoi(argv[++i]); }
        else if (strcmp(argv[i], "-range") == 0 && i+1 < argc) { range = atof(argv[++i]); }
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("Paraboloid: z = x^2 + y^2");

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special_keys);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    /* Background */
    GLfloat bg[] = {0.95f, 0.95f, 0.95f, 1.0f};
    glClearColor(bg[0], bg[1], bg[2], bg[3]);

    glutMainLoop();
    return 0;
}
