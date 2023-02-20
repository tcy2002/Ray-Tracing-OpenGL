#include <GL/glew.h>
#include <GL/freeglut.h>

#include "config/config.h"
#include "scene/scene.h"

//光线追踪场景
Scene *scene;

//记录屏幕高度
int height = ORI_HEIGHT;

void myInit() {
    scene = new Scene();
}

void myDelete() {
    delete scene;
}

void reshape(int, int h) {
    height = h;
}

void display() {
    scene->render();
    glutSwapBuffers();
}

void frame() {
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (state != 1) return;
    if (button == 0) {
        y -= (height - ORI_HEIGHT);
        auto sx = (GLfloat)(2.0 * x / ORI_WIDTH - 1.0);
        auto sy = (GLfloat)(1.0 - 2.0 * y / ORI_HEIGHT);
        scene->hitModel(sx, sy);
    } else if (button == 2) {
        scene->incIter();
    }
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(ORI_WIDTH, ORI_HEIGHT);
    glutCreateWindow("ray tracing - tcy");

    glewInit();
    myInit();

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(frame);
    glutMouseFunc(mouse);

    glutMainLoop();

    myDelete();

    return 0;
}
