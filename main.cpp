/*
 * GLUT Shapes Demo
 *
 * Written by Nigel Stewart November 2003
 *
 * This program is test harness for the sphere, cone
 * and torus shapes in GLUT.
 *
 * Spinning wireframe and smooth shaded shapes are
 * displayed until the ESC or q key is pressed.  The
 * number of geometry stacks and slices can be adjusted
 * using the + and - keys.
 */

#include <bits/stdc++.h>
using namespace std;

// #include "windows.h" // for windows

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <GL/freeglut.h>


static int slices = 16;
static int stacks = 16;

mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
// returns a random real number in [l, r] range
inline double gen_random(double l, double r) {
    return uniform_real_distribution<double>(l, r)(rng);
}

const double width = 0.1, PI  = acos(-1.0), radius = 0.05;
double cur_x = 0.0, cur_y = 1.0;
auto current_time = clock();
vector <pair<double, double>> bombs;
bool GAME_FREEZE = false;
int step = 0;

inline bool isValid(double x) {
    return x>=-1.0 && x<=1.0;
}

void showStepCount()
{
    glColor3f(1.0f, 1.0f, 1.0f);

    glRasterPos2f(0.6, 0.9);

    unsigned char msg[20] = "Step Count: ";

    // Number to string
    int tmp = step;
    string digit;
    while(tmp) {
        digit += tmp%10 + '0';
        tmp/=10;
    }
    reverse(digit.begin(), digit.end());
    if(digit.empty()) digit += "0";

    int idx = 12;
    for(auto &d : digit) msg[idx++] = d;
    msg[idx] = '\0';

    glutBitmapString(GLUT_BITMAP_HELVETICA_18, msg);
}

void showTextMessage(const unsigned char msg[], char ch)
{
    if(ch == 'g') glColor3f(0.0f, 1.0f, 0.0f);
    if(ch == 'r') glColor3f(1.0f, 0.0f, 0.0f);

    glRasterPos2f(-0.3, 0);

    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, msg);
}

void create_me(double lx, double ly)
{
    showStepCount();

    // The Square
    glBegin(GL_POLYGON);
        glColor3f(1.0, 0.0, 0.0);
        glVertex2f(lx, ly);

        glVertex2f(lx+width, ly);

        glColor3f(0.0, 0.0, 1.0);
        glVertex2f(lx+width, ly-width);

        glColor3f(0.0, 1.0, 0.0);
        glVertex2f(lx, ly-width);
    glEnd();

    // Thin Green Line
    glBegin(GL_POLYGON);
        glColor3f(0.0, 1.0, 0.0);
        glVertex2f(-1.0, -0.98);
        glVertex2f(1.0, -0.98);
        glVertex2f(1.0, -1.0);
        glVertex2f(-1.0, -1.0);
    glEnd();
}

double sq(double x) {return x*x;}

double dist(double x1, double y1, double x2, double y2) {
    return sqrtl(sq(x1-x2) + sq(y1-y2));
}

bool killed(double x, double y)
{
    double sq_cx = cur_x + width/2, sq_cy = cur_y - width/2;
    double sq_r = width/sqrtl(2);

    if(dist(x, y, sq_cx, sq_cy) <= sq_r+radius) return true;
    else return false;
}

void drawFilledCircle(double x, double y, double radius){
    int i;
    int triangleAmount = 20; //# of triangles used to draw circle

    //GLfloat radius = 0.8f; //radius
    double twicePi = 2.0f * PI;

    glBegin(GL_TRIANGLE_FAN);
        glColor3f(1.0, 0.0, 0.0);

        glVertex2f(x, y); // center of circle
        for(i = 0; i <= triangleAmount; i++)
        {
            glVertex2f(
                x + (radius * cos(i *  twicePi / triangleAmount)),
                y + (radius * sin(i * twicePi / triangleAmount))
            );
        }
    glEnd();
}

void render_bomb(double x, double y)
{
    drawFilledCircle(x, y, radius);
}

void create_bomb(int time)
{
    if(GAME_FREEZE) return;
    cerr << "BOMB!!!" << endl;

    glClear(GL_COLOR_BUFFER_BIT);
    bombs.clear();

    // Creating ME AGAIN
    create_me(cur_x, cur_y);

    int bomb_cnt = 10;
    while(bomb_cnt--) {
        double x = gen_random(-0.9, 0.9), y = gen_random(-0.9, 0.9);
        //cerr << x << " " << y << endl;
        render_bomb(x, y);
        bombs.push_back({x, y});
    }

    for(auto &[x, y] : bombs) {
        if(killed(x, y)) {
            cerr << "GAME OVER!!" << endl;

            GAME_FREEZE = true;
            const unsigned char msg[] = "YOU HAVE LOST!!!";
            showTextMessage(msg, 'r');
        }
    }

    glutSwapBuffers();

    // For Timer
    current_time = clock();
    glutTimerFunc(1000, create_bomb, current_time);
}

void gen_display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    create_me(cur_x, cur_y);
    glutSwapBuffers();
}

void game_over(int time) {
    exit(0);
}

void updateOnKeyPress()
{
    glClear(GL_COLOR_BUFFER_BIT);

    step++;

    if(cur_y-width <= -0.98) {
        cerr << "REACHED!" << endl;
        GAME_FREEZE = true;

        const unsigned char msg[] = "YOU HAVE WON!!!";
        showTextMessage(msg, 'g');
    }

    create_me(cur_x, cur_y);
    for(auto &[x, y] : bombs) render_bomb(x, y);

    for(auto &[x, y] : bombs) {
        if(killed(x, y)) {
            GAME_FREEZE = true;
            const unsigned char msg[] = "YOU HAVE LOST!!!";
            showTextMessage(msg, 'r');
        }
    }

    glutSwapBuffers();
}

void keyPressed (unsigned char key, int x, int y) {
    if(GAME_FREEZE) return;

    if(key == 'd') {
        cerr << "Right" << endl;

        if(isValid(cur_x+width + width)) cur_x += width;
        updateOnKeyPress();
    }
    else if(key == 'a') {
        cerr << "Left" << endl;

        if(isValid(cur_x-width)) cur_x -= width;
        updateOnKeyPress();
    }
    else if(key == 'w') {
        cerr << "Up" << endl;

        if(isValid(cur_y+width)) cur_y += width;
        updateOnKeyPress();
    }
    else if(key == 's') {
        cerr << "Down" << endl;

        if(isValid(cur_y-width - width)) cur_y -= width;
        updateOnKeyPress();
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc,argv);
//    glutInitDisplayMode(GLUT_SINGLE);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(700,700);
    glutInitWindowPosition(500,0);
    glutCreateWindow("RUN...");


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glutKeyboardFunc(keyPressed); // Tell GLUT to use the method "keyPressed" for key presses

    glutDisplayFunc(gen_display);

    glutTimerFunc(500, create_bomb, current_time);

    glutMainLoop();

    return 0;
}
