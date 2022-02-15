#include "3d_dat.h"

// Below must be used only in this file.
Point* mpoint;
Face* mface;

GLuint loadDAT(const char* fname) {
    FILE* f1;
    char s[81];

    int i;
    int pnum;
    int fnum;

    if (mpoint) free(mpoint);
    if (mface) free(mface);

    if ((f1 = fopen(fname, "rt")) == NULL) {
        printf("No file\n");
        exit(0);
    }

    fscanf(f1, "%s", s);
    fscanf(f1, "%s", s);
    fscanf(f1, "%d", &pnum);

    mpoint = (Point*)calloc(pnum, sizeof(Point));

    for (i = 0; i < pnum; i++) {
        fscanf(f1, "%f", &mpoint[i].x);
        fscanf(f1, "%f", &mpoint[i].y);
        fscanf(f1, "%f", &mpoint[i].z);
    }

    fscanf(f1, "%s", s);
    fscanf(f1, "%s", s);
    fscanf(f1, "%d", &fnum);

    mface = (Face*)calloc(fnum, sizeof(Face));
    for (i = 0; i < fnum; i++) {
        fscanf(f1, "%d", &mface[i].ip[0]);
        fscanf(f1, "%d", &mface[i].ip[1]);
        fscanf(f1, "%d", &mface[i].ip[2]);
    }
    fclose(f1);

    GLuint new_list = glGenLists(1);
    glNewList(new_list, GL_COMPILE);

    for (i = 0; i < fnum; i++) {
        Point* norm = cnormal(mpoint[mface[i].ip[2]], mpoint[mface[i].ip[1]], mpoint[mface[i].ip[0]]);
        glBegin(GL_TRIANGLES);
        glNormal3f(norm->x, norm->y, norm->z);
        glVertex3f(mpoint[mface[i].ip[0]].x, mpoint[mface[i].ip[0]].y, mpoint[mface[i].ip[0]].z);
        glVertex3f(mpoint[mface[i].ip[1]].x, mpoint[mface[i].ip[1]].y, mpoint[mface[i].ip[1]].z);
        glVertex3f(mpoint[mface[i].ip[2]].x, mpoint[mface[i].ip[2]].y, mpoint[mface[i].ip[2]].z);
        glEnd();
    }

    glEndList();

    return new_list;
}

GLuint loadSOR() {
    if (mpoint) free(mpoint);
    if (mface) free(mface);

    int i;
    int IncAngle = 10;
    int pnum = (360 / IncAngle) * 2;
    mpoint = (Point*)calloc(pnum, sizeof(Point));

    mpoint[0].x = 100;
    mpoint[0].y = -100;
    mpoint[0].z = 100;
    mpoint[1].x = 100;
    mpoint[1].y = 100;
    mpoint[1].z = 100;

    for (i = 0; i < pnum - 2; i++) {
        mpoint[i + 2].x = cos(IncAngle * M_PI / 180) * mpoint[i].x - sin(IncAngle * M_PI / 180) * mpoint[i].z;
        mpoint[i + 2].y = mpoint[i].y;
        mpoint[i + 2].z = sin(IncAngle * M_PI / 180) * mpoint[i].x + cos(IncAngle * M_PI / 180) * mpoint[i].z;
    }

    int fnum = 2 * 360 / IncAngle;
    mface = (Face*)calloc(fnum, sizeof(Face));
    for (i = 0; i < pnum; i += 2) {
        mface[i].ip[0] = i % pnum;
        mface[i].ip[2] = (i + 1 + 2) % pnum;
        mface[i].ip[1] = (i + 1) % pnum;

        mface[i + 1].ip[0] = i % pnum;
        mface[i + 1].ip[2] = (i + 2) % pnum;
        mface[i + 1].ip[1] = (i + 1 + 2) % pnum;

    }

    GLuint new_list = glGenLists(1);
    glNewList(new_list, GL_COMPILE);

    for (i = 0; i < fnum; i++) {
        Point* norm = cnormal(mpoint[mface[i].ip[2]], mpoint[mface[i].ip[1]], mpoint[mface[i].ip[0]]);
        glBegin(GL_TRIANGLES);
        glNormal3f(norm->x, norm->y, norm->z);
        glVertex3f(mpoint[mface[i].ip[0]].x, mpoint[mface[i].ip[0]].y, mpoint[mface[i].ip[0]].z);
        glVertex3f(mpoint[mface[i].ip[1]].x, mpoint[mface[i].ip[1]].y, mpoint[mface[i].ip[1]].z);
        glVertex3f(mpoint[mface[i].ip[2]].x, mpoint[mface[i].ip[2]].y, mpoint[mface[i].ip[2]].z);
        glEnd();
    }

    glEndList();
    return new_list;
}
