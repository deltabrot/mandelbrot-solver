#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

typedef struct {
        unsigned char r, g, b;
} rgb_t;

rgb_t **tex = 0;
int width, height;
int tex_w, tex_h;
double scale = 1.0/256.0;
double cx = -0.6, cy = 0;
int gwin;
GLuint texture;
int invert = 0;
int max_iter = 256;
int color_rotate = 0;
int saturation = 1;
double old_time = 0.0;

void render()
{
        /* struct timeval tv;
        gettimeofday(&tv, NULL);
        double diff_time = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 - old_time;
        printf("fps: %f\n", 1 / (diff_time / 1000));

        gettimeofday(&tv, NULL);
        old_time = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000; */

        double x = (double)width;
        double y = (double)height;

        glClear(GL_COLOR_BUFFER_BIT);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2i(0, 0);
        glTexCoord2f(x, 0); glVertex2i(width, 0);
        glTexCoord2f(x, y); glVertex2i(width, height);
        glTexCoord2f(0, y); glVertex2i(0, height);
        glEnd();

        glFlush();
        glFinish();
}

void hsv_to_rgb(int hue, int min, int max, rgb_t *p)
{
        if (min == max) max = min + 1;
        if (invert) hue = max - (hue - min);
        if (!saturation) {
                p->r = p->g = p->b = 255 * (max - hue) / (max - min);
                return;
        }
        double h = fmod(color_rotate + 1e-4 + 4.0 * (hue - min) / (max - min),
                        6);
#       define VAL 255
        double c = VAL * saturation;
        double X = c * (1 - fabs(fmod(h, 2) - 1));

        p->r = p->g = p->b = 0;

        switch((int)h) {
                case 0: p->r = c; p->g = X; return;
                case 1:	p->r = X; p->g = c; return;
                case 2: p->g = c; p->b = X; return;
                case 3: p->g = X; p->b = c; return;
                case 4: p->r = X; p->b = c; return;
                default: p->r = c; p->b = X;
        }
}

void calc_mandel()
{
        int i, j, iter, min, max;
        rgb_t *px;
        double x, y, zx, zy, zx2, zy2;
        min = max_iter; max = 0;
        for (i = 0; i < height; i++) {
                px = tex[i];
                y = (i - height/2) * scale + cy;
                for (j = 0; j  < width; j++, px++) {
                        x = (j - width/2) * scale + cx;
                        iter = 0;

                        zx = hypot(x - .25, y);
                        if (x < zx - 2 * zx * zx + .25) iter = max_iter;
                        if ((x + 1)*(x + 1) + y * y < 1/16) iter = max_iter;

                        zx = zy = zx2 = zy2 = 0;
                        for (; iter < max_iter && zx2 + zy2 < 4; iter++) {
                                zy = 2 * zx * zy + y;
                                zx = zx2 - zy2 + x;
                                zx2 = zx * zx;
                                zy2 = zy * zy;
                        }
                        if (iter < min) min = iter;
                        if (iter > max) max = iter;
                        *(unsigned short *)px = iter;
                }
        }

        for (i = 0; i < height; i++)
                for (j = 0, px = tex[i]; j  < width; j++, px++)
                        hsv_to_rgb(*(unsigned short*)px, min, max, px);
}

void alloc_tex()
{
        int i, ow = tex_w, oh = tex_h;

        for (tex_w = 1; tex_w < width;  tex_w <<= 1);
        for (tex_h = 1; tex_h < height; tex_h <<= 1);

        if (tex_h != oh || tex_w != ow)
                tex = realloc(tex, tex_h * tex_w * 3 + tex_h * sizeof(rgb_t*));

        for (tex[0] = (rgb_t *)(tex + tex_h), i = 1; i < tex_h; i++)
                tex[i] = tex[i - 1] + tex_w;
}

void set_texture()
{
        alloc_tex();
        calc_mandel();

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, tex_w, tex_h, 0,
                        GL_RGB, GL_UNSIGNED_BYTE, tex[0]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        render();
}

void resize(int w, int h)
{
        width = w;
        height = h;

        glViewport(0, 0, w, h);
        glOrtho(0, w, 0, h, -1, 1);

        set_texture();
}

int main(int argc, char **argv)
{
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGB);
        glutInitWindowSize(640, 480);

        gwin = glutCreateWindow("Mandelbrot");
        glutDisplayFunc(render);
        glutReshapeFunc(resize);
        glGenTextures(1, &texture);
        set_texture();

        glutMainLoop();

        return 0;
}
