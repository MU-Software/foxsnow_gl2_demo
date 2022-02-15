/*
 * Written by MUsoftware @ Hanshin Univ. in 2020.
 *
 * This is the lightweight Foxsnow that supports OpenGL2.
 * Original foxsnow uses SDL2/GLew/OpenGL 3 Core or above,
 * so some modifications and  are added to support OpenGL 2
 */

#define _USE_MATH_DEFINES
#define fRadius 10 /* È¸Àü °¢µµ */

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <GL/glut.h>

#include "src/datatype/fs_node.h"
#include "src/fs_node_render.h"


GLfloat mKa[4] = { 0.2f, 0.2f, 0.2f, 0.0f }; /* Object : Ambient */
GLfloat mKd[4] = { 0.6f, 0.6f, 0.6f, 0.0f }; /* Object : Diffuse */
GLfloat mKs[4] = { 0.2f, 0.2f, 0.2f, 0.0f }; /* Object : Specular */
GLfloat mKe[4] = { 0.0f, 0.0f, 0.0f, 0.0f };  /* Object : Emission */
GLfloat shininess[1] = { 50 };  /* Object : Shininess */

Point Pos, Dir;
double rads = 1.5708;
bool isColliding = false;

int current_screen_x = 800, current_screen_y = 600;

bool mode_bird_eye = false;
bool mode_full_screen = false;

bool mode_cull = true;
bool mode_wireframe = false;
bool mode_texture = true;

void* font = GLUT_BITMAP_HELVETICA_12;
#define LINE_HEIGHT 18

node* changeable_node = NULL;
node* bunny_node = NULL;
node* healthkit_node = NULL;
node *solar_system_node = NULL , *solar_system_sun_node = NULL;
node *solar_system_earth_rev_node = NULL, *solar_system_earth_rot_node = NULL;
node *solar_system_moon_rev_node = NULL, *solar_system_moon_rot_node = NULL;

float verticalSpeed = 0.0f;
void playerMovementJump(int _no_use) {
    if (Pos.y > 5.0f && Pos.y + verticalSpeed > 5.0f) {
        // This will make player up for a moment and down.
        // I got this idea from Super Mario 64(Nintendo, 1999)
        // Check this video https://youtu.be/3lADTcTkAvk?t=201
        Pos.y += verticalSpeed;
        verticalSpeed -= 0.1f;
        glutPostRedisplay();
        glutTimerFunc(1000 / 60, playerMovementJump, 0);
    }
    else {
        // Stop player vertically moving!
        verticalSpeed = 0;
        Pos.y = 5.0f;
        glutPostRedisplay();
    }
}

bool setCrouch = false;
void playerMovementCrouch(int _no_use) {
    if (setCrouch) {
        if (Pos.y > 2.25f) Pos.y -= 0.25;
        glutTimerFunc(1000 / 60, playerMovementCrouch, 0);
    }
    else {
        if (Pos.y < 5.0f) {
            Pos.y += 0.3;
            glutTimerFunc(1000 / 60, playerMovementCrouch, 0);
        }
        else {
            Pos.y = 5.0f;
        }
    }
    glutPostRedisplay();
}

bool boostSpeed = false;
float movementSpeed = 0.3f;
float xMovementSpeed = 0.0f;
float yMovementSpeed = 0.0f;
void playerMovementLoopHandler(int _no_use) {
    Point prevPos = Pos;

    GLdouble xDelta, zDelta;
    double frads = M_PI * 0.5 - rads;
    float resultXSpeed = xMovementSpeed * (boostSpeed ? 2.0f : 1.0f) * (setCrouch ? 0.3f : 1.0f);
    float resultYSpeed = yMovementSpeed * (boostSpeed ? 2.0f : 1.0f) * (setCrouch ? 0.3f : 1.0f);
    xDelta = resultXSpeed * cos(rads) + resultYSpeed * cos(frads);
    zDelta = -resultXSpeed * sin(rads) + resultYSpeed * sin(frads);
    Pos.x += (float)xDelta;
    Pos.z += (float)zDelta;

    if (fs_renderNode_isCollide(render.node_start, Pos, 1.25f)) {
        // If player is colliding with something, then revert movement.
        Pos = prevPos;
        isColliding = true;
    } else {
        isColliding = false;
    }

    glutPostRedisplay();
    glutTimerFunc(1000 / 60, playerMovementLoopHandler, 0);
}

float cylindricalYLookAtPos = 0;
void mouseMove(int x, int y) {
    rads -= (x - (current_screen_x / 2)) * 0.005f;

    float old_cylindricalYLookAtPos = cylindricalYLookAtPos;
    cylindricalYLookAtPos -= (y - (current_screen_y / 2)) * 0.02f;
    if (-15.0f > cylindricalYLookAtPos || cylindricalYLookAtPos > 25.0f)
        cylindricalYLookAtPos = old_cylindricalYLookAtPos;

    glutPostRedisplay();
    glutWarpPointer(current_screen_x / 2, current_screen_y / 2);
}

void keyboardDown(int key, int x, int y) {
    float target_speed = (glutGetModifiers() == GLUT_ACTIVE_CTRL) ? movementSpeed * 2 : movementSpeed;
    switch (key) {
    // Reset position
    case '0':
        Pos.x = Pos.z = 0;
        Dir.x = Dir.z = 0;
        cylindricalYLookAtPos = 0;
        Dir.y = 0;
        glutPostRedisplay();
        break;

    // Walk & Run
    case 'w':
        xMovementSpeed = target_speed;
        break;
    case 'a':
        yMovementSpeed = -target_speed;
        break;
    case 's':
        xMovementSpeed = -target_speed;
        break;
    case 'd':
        yMovementSpeed = target_speed;
        break;

    case 'c':  // Crouch
        setCrouch = true;
        playerMovementCrouch(0);
        break;
    case ' ':  // Jump
        if (!verticalSpeed && !setCrouch) {
            verticalSpeed = 1.5f;
            Pos.y = 5.01f;
            playerMovementJump(0);
        }
        break;

    // Node replacement(change demo)
    case '1':  // Show character collide demo
        changeable_node->child = changeable_node->child_last = bunny_node;
        break;
    case '2':  // Show health kit demo
        // Attach health kit demo node
        changeable_node->child = changeable_node->child_last = healthkit_node;
        break;
    case '3':  // Show solar system demo
        // Attach solar system demo node
        changeable_node->child = changeable_node->child_last = solar_system_node;
        break;


    case 27:
        exit(EXIT_SUCCESS);
        break;
    }

    glutPostRedisplay();
}

void keyboardUp(int key, int x, int y) {
    switch (key) {
    case 'w':
    case 's':
        xMovementSpeed = 0.0;
        break;
    case 'a':
    case 'd':
        yMovementSpeed = 0.0;
        break;

    case 'c':
        setCrouch = false;
        break;
    }
}

void functionKeyDown(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_F1:
        mode_texture = !mode_texture;
        printf("%s texture\n", mode_texture ? "Enable" : "Disable");
        break;
    case GLUT_KEY_F2:
        mode_bird_eye = !mode_bird_eye;
        printf("%s bird eye camera\n", mode_bird_eye ? "Enable" : "Disable");
        break;
    case GLUT_KEY_F3:
        mode_cull = !mode_cull;
        printf("%s culling\n", mode_cull ? "Enable" : "Disable");
        break;

    case GLUT_KEY_F11:
        if (!mode_full_screen) {
            glutFullScreen();
            printf("FullScreen\n");
            mode_full_screen = true;
        }
        else {
            glutPositionWindow(100, 100);
            glutReshapeWindow(800, 600);
            printf("Window\n");
            mode_full_screen = false;
        }
        break;
    case GLUT_KEY_F12:
        mode_wireframe = !mode_wireframe;
        printf("%s wireframe mode\n", mode_wireframe ? "Enable" : "Disable");
        break;


    default:
        if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {
            boostSpeed = !boostSpeed;
            printf("%s\n", boostSpeed ? "´Þ¸®±â ¸ðµå" : "°È±â ¸ðµå");
        }
        break;
    }

    glutPostRedisplay();
}


// These functions are used in demo scenes
void demoSceneMovement(int _no_use) {
    static unsigned long long frame = 0;


    // Health kit demo animating
    fs_3d_data* healthkit_node_data = (fs_3d_data*)healthkit_node->data;
    healthkit_node_data->rotate[1] = frame % 360;


    // Solar system demo animating
    fs_3d_data* solar_system_sun_node_data = (fs_3d_data*)solar_system_sun_node->data;
    solar_system_sun_node_data->rotate[1] = (frame * 6) % 360;
    fs_3d_data* solar_system_earth_rot_node_data = (fs_3d_data*)solar_system_earth_rot_node->data;
    solar_system_earth_rot_node_data->rotate[1] = (frame * 7) % 360;
    fs_3d_data* solar_system_earth_rev_node_data = (fs_3d_data*)solar_system_earth_rev_node->data;
    solar_system_earth_rev_node_data->rotate[1] = (frame / 2) % 360;
    fs_3d_data* solar_system_moon_rot_node_data = (fs_3d_data*)solar_system_moon_rot_node->data;
    solar_system_moon_rot_node_data->rotate[1] = (frame * 3) % 360;
    fs_3d_data* solar_system_moon_rev_node_data = (fs_3d_data*)solar_system_moon_rev_node->data;
    solar_system_moon_rev_node_data->rotate[1] = (frame * 4) % 360;


    frame++;
    glutPostRedisplay();
    glutTimerFunc(1000 / 60, demoSceneMovement, 0);
}

void GLSetupLight(void) {
    GLfloat lmodel_ambient[] = {
      0.0,
      0.0,
      0.0,
      1.0
    }; // Ambient of the entire scene
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

    /* Enable a Single Directional light. */
    GLfloat light0_ambient[] = {
      0.2f,
      0.2f,
      0.2f,
      1.0f
    }; // Ambient light
    GLfloat light0_diffuse[] = {
      1.0,
      1.0,
      1.0,
      1.0
    }; // White diffuse light
    GLfloat light0_specular[] = {
      1.0,
      1.0,
      1.0,
      1.0
    }; // Specular light
    GLfloat light0_position[] = {
      1.0,
      1.0,
      1.0,
      0.0
    }; // Infinite light location

    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glEnable(GL_LIGHT0);

    GLfloat light1_ambient[] = {
      0.2f,
      0.2f,
      0.2f,
      1.0f
    }; // Ambient light
    GLfloat light1_diffuse[] = {
      1.0,
      1.0,
      1.0,
      1.0
    }; // White diffuse light
    GLfloat light1_specular[] = {
      1.0,
      1.0,
      1.0,
      1.0
    }; // Specular light
    GLfloat light1_position[] = {
      0.0,
      1.0,
      0.0,
      1.0
    }; // Point light location

    // Enable a single OpenGL light.
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0); // °¨¼èÇö»ó  (Constant: ca)
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0); // °¨¼èÇö»ó  (Linear: la)
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0); // °¨¼èÇö»ó  (Quadric: qa)
    glEnable(GL_LIGHT1);

    GLfloat light2_ambient[] = {
      0.2,
      0.2,
      0.2,
      1.0
    };
    GLfloat light2_diffuse[] = {
      1.0,
      1.0,
      1.0,
      1.0
    };
    GLfloat light2_specular[] = {
      1.0,
      1.0,
      1.0,
      1.0
    };
    GLfloat light2_position[] = {
      1.0,
      0.0,
      0.0,
      1.0
    };
    GLfloat spot_direction[] = {
      0.0,
      0.0,
      -1.0
    };

    glLightfv(GL_LIGHT2, GL_AMBIENT, light2_ambient); // ±¤¿ø ÁÖº¯±¤ (Intensity: I0)
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse); // ±¤¿ø ³­¹Ý»ç  (Intensity: I0)
    glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular); // ±¤¿ø Á¤¹Ý»ç  (Intensity: I0)
    glLightfv(GL_LIGHT2, GL_POSITION, light2_position); // ±¤¿ø À§Ä¡      (Position: p)

    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0); // °¨¼èÇö»ó  (Constant: ca)
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.0); // °¨¼èÇö»ó  (Linear: la)
    glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.0); // °¨¼èÇö»ó  (Quadric: qa)

    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction); // ÁýÁß±¤¿ø ¹æÇâ (Direction: d)
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 2.0); // ÁýÁß±¤¿ø (Falloff: sd)
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 45.0); // ÁýÁß±¤¿ø (Cutoff : sc)

    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHTING);
}


int playerHealth = 10;
void restoreHealthkit(int _no_use);
bool isHealthkitCollide(node* target, Point player, float player_radius) {
    if (!target) return false;
    fs_3d_data* target_data = (fs_3d_data*)target->data;
    if (!target_data) return false;

    // Get distance of target and player
    // We just check 2d collide, so just use X and Z
    float x_ = pow(target_data->pos[0] - player.x, 2.0f);
    float y_ = pow(target_data->pos[2] - player.z, 2.0f);
    double dist = sqrt(x_ + y_);

    // Compare distance with sum of radii (player radius and target radius)
    if (dist <= (target_data->collision_radius_2d * pow(target_data->scale[0], 2.0) + player_radius)) {
        target_data->scale[0] = target_data->scale[1] = target_data->scale[2] = 0.0f;
        target_data->collision_check = NULL;
        playerHealth += 100;
        printf("Collided with HPkit\n");
        glutTimerFunc(5000, restoreHealthkit, 0);
    }
    return false;  // This must be false always.
}
void restoreHealthkit(int _no_use) {
    fs_3d_data* target_data = (fs_3d_data*)healthkit_node->data;
    target_data->scale[0] = target_data->scale[1] = target_data->scale[2] = 0.1f;
    target_data->collision_check = isHealthkitCollide;
    printf("HPkit restored!\n");
}


void render2DText() {
    // Setup text environment
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)current_screen_x, 0.0, (GLdouble)current_screen_y);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);

    // Render text
    glPushAttrib(GL_CURRENT_BIT);
    char* player_hp_status = (char*)calloc(64, sizeof(char));
    ALLOC_FAILCHECK(player_hp_status);
    sprintf(player_hp_status, "  > Player HP : %4d", playerHealth);
    char* static_char_collection[] = {
        "Status",
        isColliding ? "  > Player is colliding into something" : "  > Player is not colliding",
        boostSpeed ? "  > Running mode" : "  > Walking mode",
        player_hp_status,
        " ",
        " ",
        "Demo select(Press 1-3)",
        "  > 1 : Collision blocking: Stanford bunny",
        "  > 2 : Collision checking: Healthkit Pick",
        "  > 3 : Scene tree demo: Inaccurate solar system",
        " ",
        "Move: W/A/S/D   | Toggle Run mode: Ctrl",
        "Jump: Space       | Crouch: C",
        "Camera move: Mouse movement",
        "Reset camera: 0",
        " ",
        "Toggle Texture: F1",
        "Toggle Bird-Eye-Camera: F2",
        "Toggle Culling: F3",
        "Fullscreen: F11 | Wireframe: F12",
        " ",
        "Exit: ESC",
    };
    int static_char_collection_len = sizeof(static_char_collection) / sizeof(char*);
    int index = 0;
    for (index; index < static_char_collection_len; index++) {
        char *c = static_char_collection[index];
        glColor3f(0.0f, 1.0f, 0.0f);
        glRasterPos2i(20, LINE_HEIGHT * (static_char_collection_len - index));
        for (c; *c != '\0'; c++) glutBitmapCharacter(font, *c);
    }
    free(player_hp_status);
    glPopAttrib();

    // Unset text environment
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}


// Setup engine
void fs_setup() {
    // Use depth buffering for hidden surface elimination
    glEnable(GL_DEPTH_TEST);

    // Use three lights and enables it
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHTING);

    // Use material
    glEnable(GL_COLOR_MATERIAL);

    // Ask OpenGL to automatically re-normalize
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_SMOOTH);

    // Use alpha blend function, this is used in texture alpha.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // This is deprecated, but we cannot use fragment shader, so we'll replace with this.
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_NOTEQUAL, 0.0f);
    

    // Setup lights
    GLSetupLight();


    // Set camera position
    Pos.x = 0.0f, Pos.y = 5.0f, Pos.z = 0.0f;


    // Initialize root node
    fs_3d_data* root_data = (fs_3d_data*)calloc(1, sizeof(fs_3d_data));
    render.node_start = create_node(
                            NULL,
                            renderNodeIn,
                            renderNodeOut,
                            root_data,
                            strdup("ROOT NODE"));
}

void fs_renderer() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    current_screen_x = glutGet(GLUT_WINDOW_WIDTH);
    current_screen_y = glutGet(GLUT_WINDOW_HEIGHT);
    double current_screen_ratio = (double)current_screen_x / (double)current_screen_y;

    // Reset projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
        60.0, // field of view in degree
        current_screen_ratio, // aspect ratio
        1.0, // Z near
        2000.0); // Z far



    // Reset ModelView matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Camera position calculation(Cylindrical Coordinates)
    Dir.x = Pos.x + (fRadius * cos(rads));
    Dir.y = Pos.y + cylindricalYLookAtPos;
    Dir.z = Pos.z - (fRadius * sin(rads));

    // Set view matrix
    if (mode_bird_eye) {
        gluLookAt(Pos.x, Pos.y + 1000, Pos.z,
                  Dir.x, Dir.y,        Dir.z,
                  0.0f,  1.0f,         0.0f);
    }
    else {
        gluLookAt(Pos.x, Pos.y, Pos.z,
                  Dir.x, Dir.y, Dir.z,
                  0.0f,  1.0f,  0.0f);
    }



    // Setup draw modes (culling / texture / wireframe)
    if (mode_cull) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);

    if (mode_texture) glEnable(GL_TEXTURE_2D);
    else glDisable(GL_TEXTURE_2D);

    glPolygonMode(GL_FRONT_AND_BACK, mode_wireframe ? GL_LINE : GL_FILL);



    // Render the render node.
    fs_renderNode_render(render.node_start);

    // Render test
    render2DText();



    // Swap buffers (Double buffering maybe?)
    glutSwapBuffers();
}

void fs_scene_setup() {
    node* room_node = fs_renderNode_appendChild(render.node_start);
    fs_renderNode_setOBJ(room_node, "res/room.fs3d");
    ((fs_3d_data*)(room_node->data))->collision_check = mapCollideCheck;


    // Demo scene setup
    changeable_node = fs_renderNode_appendChild(room_node);

    // Player collide demo (Collide detection)
    {
        bunny_node = fs_renderNode_appendChild(changeable_node);
        fs_renderNode_setOBJ(bunny_node, "res/bunny.fs3d");
        fs_3d_data* bunny_node_data = (fs_3d_data*)bunny_node->data;
        bunny_node_data->scale[0] = bunny_node_data->scale[1] = bunny_node_data->scale[2] = 0.01f;
        bunny_node_data->pos[1] = 2.5f;
        bunny_node_data->pos[2] = -16.0f;
        bunny_node_data->collision_check = sphericalCollideCheck;  // Set normal collide check handler
    }

    // Health kit demo (Collide detection gimmick)
    {
        healthkit_node = fs_renderNode_appendChild(NULL);
        fs_renderNode_setOBJ(healthkit_node, "res/medkit_medium.fs3d");
        fs_3d_data* healthkit_node_data = (fs_3d_data*)healthkit_node->data;
        healthkit_node_data->scale[0] = healthkit_node_data->scale[1] = healthkit_node_data->scale[2] = 0.1f;
        healthkit_node_data->pos[2] = -16.0f;
        healthkit_node_data->collision_check = isHealthkitCollide;  // Set health kit collide handler
    }

    // Solar system demo node, this will be HUGE!
    {
        solar_system_node = fs_renderNode_appendChild(NULL);
        ((fs_3d_data*)solar_system_node->data)->pos[1] = 3.0f;
        ((fs_3d_data*)solar_system_node->data)->pos[2] = -24.0f;

        // Solar system - sun
        solar_system_sun_node = fs_renderNode_appendChild(solar_system_node);
        fs_renderNode_setOBJ(solar_system_sun_node, "res/0_sun.fs3d");
        fs_3d_data* solar_system_sun_node_data = (fs_3d_data*)solar_system_sun_node->data;
        solar_system_sun_node_data->scale[0] = solar_system_sun_node_data->scale[1] = solar_system_sun_node_data->scale[2] = 0.03f;

        // Solar system - earth
        solar_system_earth_rev_node = fs_renderNode_appendChild(solar_system_node);  // Revolution, °øÀü in korean
        solar_system_earth_rot_node = fs_renderNode_appendChild(solar_system_earth_rev_node);  // Rotation, ÀÚÀü in korean
        fs_renderNode_setOBJ(solar_system_earth_rot_node, "res/3_0_earth.fs3d");
        fs_3d_data* solar_system_earth_rot_node_data = (fs_3d_data*)solar_system_earth_rot_node->data;
        solar_system_earth_rot_node_data->scale[0] = solar_system_earth_rot_node_data->scale[1] = solar_system_earth_rot_node_data->scale[2] = 0.0125f;
        solar_system_earth_rot_node_data->pos[2] = 5.0f;  // Distance from sun

        // Solar system - moon
        solar_system_moon_rev_node = fs_renderNode_appendChild(solar_system_earth_rot_node);  // Revolution
        solar_system_moon_rot_node = fs_renderNode_appendChild(solar_system_moon_rev_node);  // Rotation
        fs_renderNode_setOBJ(solar_system_moon_rot_node, "res/3_1_moon.fs3d");
        fs_3d_data* solar_system_moon_rot_node_data = (fs_3d_data*)solar_system_moon_rot_node->data;
        solar_system_moon_rot_node_data->scale[0] = solar_system_moon_rot_node_data->scale[1] = solar_system_moon_rot_node_data->scale[2] = 0.005f;
        solar_system_moon_rot_node_data->pos[2] = 1.5f;  // Distance from earth

    }


    demoSceneMovement(0);
    playerMovementLoopHandler(0);
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glListBase(1); // Start list index at 1, because we want to use 0 as 'Not normal list state'

    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Foxsnow GL2 demo");

    glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(functionKeyDown);

    glutSetCursor(GLUT_CURSOR_NONE);
    glutPassiveMotionFunc(mouseMove);

    glutDisplayFunc(fs_renderer);


    fs_setup();
    fs_scene_setup();

    glutMainLoop();

    return EXIT_SUCCESS;
}
