#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>

int width = 1920;
int height = 1080;

struct Type_rgb{
    float r;
    float g;
    float b;
};
struct Type_rgb pixels[1920*1080], pattern[999];

void mandelbrotset()
{
    float x0, y0, x, y, xtemp;
    int iteration, max_iteration, loc=0;
    printf("\nstart");
    for(y0 = -1; y0 < 1.1; y0 = y0 + 0.0025)
        for(x0 = -2.5; x0 < 1.1; x0 = x0 + 0.0025){
            x = 0;
            y = 0;
            iteration = 0;
            max_iteration = 1000;

            while(((x * x) + (y * y) < (2 * 2)) && iteration < max_iteration){
                xtemp = (x * x) - (y * y) + x0;
                y = (2 * x * y) + y0;

                x = xtemp;
                iteration = iteration + 1;
            }
            if(iteration >= 999){
                pixels[loc].r = 0;
                pixels[loc].g = 0;
                pixels[loc].b = 0;
            } else {
                pixels[loc].r = pattern[iteration].r;
                pixels[loc].g = pattern[iteration].g;
                pixels[loc].b = pattern[iteration].b;
            }
            loc = loc + 1;
        }
}

void init( )
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity( );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity( );
    gluOrtho2D(0, width, 0, height);

    int i;
    float r, g, b;
    for(i = 0; i < width*height; i++){
        pixels[i].r = 1;
        pixels[i].g = 1;
        pixels[i].b = 1;
    }

    i = 0;
    for(r = 0.1; r <= 0.9; r= r+0.1)
        for(g = 0.1; g <= 0.9; g = g+0.1)
            for(b = 0.1; b <= 0.9; b = b+0.1){
                pattern[i].r = b;
                pattern[i].g = r;
                pattern[i].b = g;
                i++;
            }

    for( ; i <= 999; i++){
        pattern[i].r = 1;
        pattern[i].g = 0;
        pattern[i].b = 1;
    }
    // mandelbrotset();
}

void onDisplay()
{
    glClearColor(1, 1, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(width, height, GL_RGB, GL_FLOAT, pixels);
    glutSwapBuffers();
}

void resize(int w, int h)
{
	printf("resize %d %d\n", w, h);
	width = w;
	height = h;
 
	glViewport(0, 0, w, h);
	glOrtho(0, w, 0, h, -1, 1);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(width, height);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Mandelbrot Set");

    init();
    glutDisplayFunc(onDisplay);
    glutReshapeFunc(resize);
    glutMainLoop();
    return 0;
}

