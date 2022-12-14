#include <bits/stdc++.h>
using namespace std;

#include <GL/freeglut.h>
#include "windows.h"
#include <GL/glut.h>
#include <stdlib.h>

mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());

// returns a random real number in [l, r] range
inline double gen_random(double l, double r)
{
    return uniform_real_distribution<double>(l, r)(rng);
}

const double width = 0.1, PI  = acos(-1.0), radius = 0.05;
double cur_x = 0.0, cur_y = 1.0;
auto current_time = clock();

bool GAME_FREEZE = false;
int step = 0;

struct point
{
    double x, y;
};

vector <point> bombs;

void game_exit(int time) {
    exit(0);
}

inline bool isValid(double x)
{
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
    while(tmp)
    {
        digit += tmp%10 + '0';
        tmp/=10;
    }
    reverse(digit.begin(), digit.end());
    if(digit.empty())
        digit += "0";

    int idx = 12;
    for(auto &d : digit)
        msg[idx++] = d;
    msg[idx] = '\0';

    glutBitmapString(GLUT_BITMAP_HELVETICA_18, msg);
}

void showTextMessage(const unsigned char msg[], char ch)
{
    if(ch == 'g')
        glColor3f(0.0f, 1.0f, 0.0f);
    if(ch == 'r')
        glColor3f(1.0f, 0.0f, 0.0f);

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

double sq(double x)
{
    return x*x;
}

double dist(double x1, double y1, double x2, double y2)
{
    return sqrtl(sq(x1-x2) + sq(y1-y2));
}

bool killed(double x, double y)
{
    double sq_cx = cur_x + width/2, sq_cy = cur_y - width/2;
    double sq_r = width/sqrtl(2);

    if(dist(x, y, sq_cx, sq_cy) <= sq_r+radius) {
        //glutTimerFunc(2500, game_exit, clock());
        return true;
    }
    else
        return false;
}

void drawFilledCircle(double x, double y, double radius)
{
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
    if(GAME_FREEZE)
        return;
    cerr << "BOMB!!!" << endl;

    glClear(GL_COLOR_BUFFER_BIT);
    bombs.clear();

    // Creating ME AGAIN
    create_me(cur_x, cur_y);

    int bomb_cnt = 15;
    while(bomb_cnt--)
    {
        double x = gen_random(-1.0+radius, 1.0-radius), y = gen_random(-0.97+radius, 1.0-radius);
        //cerr << x << " " << y << endl;
        render_bomb(x, y);
        bombs.push_back({x, y});
    }

    point p;
    for(auto p : bombs)
    {
        if(killed(p.x, p.y))
        {
            cerr << "GAME OVER!!" << endl;

            GAME_FREEZE = true;
            const unsigned char msg[] = "YOU HAVE LOST!!!";
            showTextMessage(msg, 'r');
        }
    }

    glutSwapBuffers();

    // For Timer
    current_time = clock();
    glutTimerFunc(500, create_bomb, current_time);
}

void gen_display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    create_me(cur_x, cur_y);
    glutSwapBuffers();
}

void updateOnKeyPress()
{
    if(GAME_FREEZE)
        return;

    glClear(GL_COLOR_BUFFER_BIT);

    step++;

    if(cur_y-width <= -0.98)
    {
        cerr << "REACHED!" << endl;
        GAME_FREEZE = true;

        const unsigned char msg[] = "YOU HAVE WON!!!";
        showTextMessage(msg, 'g');
    }

    create_me(cur_x, cur_y);
    for(auto p : bombs)
        render_bomb(p.x, p.y);

    for(auto p : bombs)
    {
        if(killed(p.x, p.y))
        {
            GAME_FREEZE = true;
            const unsigned char msg[] = "YOU HAVE LOST!!!";
            showTextMessage(msg, 'r');
        }
    }

    glutSwapBuffers();
}

void keyPressed (unsigned char key, int x, int y)
{
    if(key == 'd' || key == 'D')
    {
        cerr << "Right" << endl;

        if(isValid(cur_x+width + width))
            cur_x += width;
        updateOnKeyPress();
    }
    else if(key == 'a' || key == 'A')
    {
        cerr << "Left" << endl;

        if(isValid(cur_x-width))
            cur_x -= width;
        updateOnKeyPress();
    }
    else if(key == 'w' || key == 'W')
    {
        cerr << "Up" << endl;

        if(isValid(cur_y+width))
            cur_y += width;
        updateOnKeyPress();
    }
    else if(key == 's' || key == 'S')
    {
        cerr << "Down" << endl;

        if(isValid(cur_y-width - width))
            cur_y -= width;
        updateOnKeyPress();
    }
    else if(key == 'e' || key == 'E' || key == (char)27) {
        game_exit(0);
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
