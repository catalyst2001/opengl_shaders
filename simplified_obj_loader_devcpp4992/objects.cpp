#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>

int main(int argc, char *argv[])
{
    FILE *File;
    
    File = fopen("cylinder.objs", "wb+");
    
    fprintf(File, "dm diffusemap3.jpg\r\n");
    fprintf(File, "nm normalmap3.jpg\r\n");

    fprintf(File, "\r\n");

    int r = 32;
    
    for(int i = 0; i < r; i++)
    {
        fprintf(File, "v %f %f %f\r\n", cos(M_PI * 2.0f * ((float)i / (float)r)) * 0.5f, -0.5f, -sin(M_PI * 2.0f * ((float)i / (float)r)) * 0.5f);
    }

    for(int i = 0; i < r; i++)
    {
        fprintf(File, "v %f %f %f\r\n", cos(M_PI * 2.0f * ((float)i / (float)r)) * 0.5, 0.5, -sin(M_PI * 2.0 * ((float)i / (float)r)) * 0.5);
    }

    fprintf(File, "v %f %f %f\r\n", 0.0f, -0.5f, 0.0f);
    fprintf(File, "v %f %f %f\r\n", 0.0f, 0.5f, 0.0f);

    fprintf(File, "\r\n");

    for(int i = 0; i <= r; i++)
    {
        fprintf(File, "tc %f %f\r\n", 3.0f * (float)i / (float)r, 0.0f);
    }

    for(int i = 0; i <= r; i++)
    {
        fprintf(File, "tc %f %f\r\n", 3.0f * (float)i / (float)r, 1.0f);
    }

    for(int i = 0; i < r; i++)
    {
        fprintf(File, "tc %f %f\r\n", 0.5f + cos(M_PI * 2.0f * ((float)i / (float)r)) * 0.5f, 0.5f - sin(M_PI * 2.0f * ((float)i / (float)r)) * 0.5f);
    }

    for(int i = 0; i < r; i++)
    {
        fprintf(File, "tc %f %f\r\n", 0.5f + cos(M_PI * 2.0f * ((float)i / (float)r)) * 0.5f, 0.5f + sin(M_PI * 2.0f * ((float)i / (float)r)) * 0.5f);
    }
    
    fprintf(File, "tc %f %f\r\n", 0.5f, 0.5f);

    fprintf(File, "\r\n");

    for(int i = 0; i < r; i++)
    {
        fprintf(File, "n %f %f %f\r\n", cos(M_PI * 2.0f * ((float)i / (float)r)), 0.0f, -sin(M_PI * 2.0f * ((float)i / (float)r)));
    }

    fprintf(File, "n %f %f %f\r\n", 0.0f, -1.0f, 0.0f);
    fprintf(File, "n %f %f %f\r\n", 0.0f, 1.0f, 0.0f);

    fprintf(File, "\r\n");

    for(int i = 0; i < r; i++)
    {
        int va = i, vb = (i + 1) % r, vc = r + ((i + 1) % r), vd = r + i;
        int tca = i, tcb = i + 1, tcc = i + (r + 1) + 1, tcd = i + (r + 1);
        int na = i, nb = (i + 1) % r, nc = (i + 1) % r, nd = i;

        fprintf(File, "t %d/0/%d/%d %d/0/%d/%d %d/0/%d/%d\r\n", va + 1, tca + 1, na + 1, vb + 1, tcb + 1, nb + 1, vc + 1, tcc + 1, nc + 1);
        fprintf(File, "t %d/0/%d/%d %d/0/%d/%d %d/0/%d/%d\r\n", vc + 1, tcc + 1, nc + 1, vd + 1, tcd + 1, nd + 1, va + 1, tca + 1, na + 1);
    }

    for(int i = 0; i < r; i++)
    {
        int va = (i + 1) % r, vb = i, vc = r * 2;
        int tca = (r + 1) * 2 + (i + 1) % r, tcb = (r + 1) * 2 + i, tcc = (r + 1) * 2 + r * 2;
        int n = r;

        fprintf(File, "t %d/0/%d/%d %d/0/%d/%d %d/0/%d/%d\r\n", va + 1, tca + 1, n + 1, vb + 1, tcb + 1, n + 1, vc + 1, tcc + 1, n + 1);
    }

    for(int i = 0; i < r; i++)
    {
        int va = r + i, vb = r + ((i + 1) % r), vc = r * 2 + 1;
        int tca = (r + 1) * 2 + r + i, tcb = (r + 1) * 2 + r + (i + 1) % r, tcc = (r + 1) * 2 + r * 2;
        int n = r + 1;

        fprintf(File, "t %d/0/%d/%d %d/0/%d/%d %d/0/%d/%d\r\n", va + 1, tca + 1, n + 1, vb + 1, tcb + 1, n + 1, vc + 1, tcc + 1, n + 1);
    }
    
    fclose(File);

    return 0;
}
