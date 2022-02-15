#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <GL/glut.h>

GLuint loadBMP(char *name) {
    FILE* BMPfile;
    GLubyte garbage;
    long size;
    int start_point,x;
    GLubyte temp[3];
    GLubyte start[4], w[4], h[4];
    unsigned char *read_image;

    int width, height;
  
    BMPfile = fopen(name, "rb");
 
    for (x = 0; x < 10; x++) {
        fread(&garbage, 1, 1, BMPfile);
    }
  
    fread(&start[0], 1, 4, BMPfile);
 
    for (x = 0; x < 4; x++) {
        fread(&garbage, 1, 1, BMPfile);
    }

    fread(&w[0], 1, 4, BMPfile);
    fread(&h[0], 1, 4, BMPfile);

    width = (w[0] + 256 * w[1] + 256 * 256 * w[2] + 256 * 256 * 256 * w[3]);
    height = (h[0] + 256 * h[1] + 256 * 256 * h[2] + 256 * 256 * 256 * h[3]);
    size = width * height;
    start_point = (start[0] + 256 * start[1] + 256 * 256 * start[2] + 256 * 1256 * 256 * start[3]);

    read_image = (unsigned char*)malloc(size * 4);

    for (x = 0; x < (start_point - 26); x++) {
        fread(&garbage, 1, 1, BMPfile);
    }

    for (x = 0; x < (size * 4); x = x + 4) {
        fread(&temp[0], 1, 3, BMPfile);
        read_image[x + 0] = temp[2];
        read_image[x + 1] = temp[1];
        read_image[x + 2] = temp[0];

        if (temp[2] == 0xFF && temp[1] == 0x00 && temp[0] == 0xFF)
            read_image[x + 3] = 0;
        else read_image[x + 3] = 255;
    }

    fclose(BMPfile);

    GLuint texture = 0;  // 0 is invalid image.
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)read_image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);

    free(read_image);

    return texture;
}


