#include "stdafx.h"
#include "main.h";
#include "draw_scene.h";

//GLOBAL: used to store the deltaX position of the cloud
float cloudDeltaX = 0.003f;

int _tmain(int argc, char* argv[]) {
	init(argc, argv);
	glutMainLoop();

	// Shut down COM
	shutdownCOM();

	return 0;
}

void init(int argc, char* argv[]) {
	// 1. Initialise FreeGLUT
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	glutInitWindowSize(800, 800);
	glutInitWindowPosition(64, 64);
	glutCreateWindow("CS2S565 - William Akins");

	// Display callback
	glutDisplayFunc(display);
	glutIdleFunc(update);
	glutKeyboardFunc(keyDown);
	glutMouseFunc(mouseButtonDown);

	// 2. Initialise GLEW library
	GLenum err = glewInit();

	// Ensure the GLEW library was initialised successfully before proceeding
	if (err == GLEW_OK) {
		std::cout << "GLEW initialised...\n";
	} else {
		std::cout << "GLEW could not be initialised!\n";
		throw;
	}

	// Example query OpenGL state (get version number)
	reportVersion();

	// Report maximum number of vertex attributes
	GLint numAttributeSlots;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttributeSlots);
	std::cout << "GL_MAX_VERTEX_ATTRIBS = " << numAttributeSlots << std::endl;

	// 3. Initialise OpenGL settings and objects we'll use in our scene
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glLineWidth(1.0f);

	//sets background colour of window to black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//Setup the textures to be used
	setupTextures();

	//Setup the shaders to be used
	setupShaders();

	//Setup the objects to be rendered
	setupSkyVAO();
	setupGroundVAO();
	setupMissileVAO();
	setupGrassVAO();
	setupMissileExplosionVAO();
	setupCloudVAO();
}

void reportVersion(void) {
	int majorVersion, minorVersion;

	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

	std::cout << "OpenGL version " << majorVersion << "." << minorVersion << "\n\n";
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw and transform the objects to screen
	drawSkyVAO();
	drawGroundVAO();
	drawMissileVAO();
	drawGrassVAO();
	drawCloudVAO();

	//If the missile has exploded draw the explosion image
	if (getMissileExploded()) {
		drawMissileExplosionVAO();
	}

	//encapsulates these commands and performs double buffering
	glutSwapBuffers();
}

// update is called every frame
void update(void) {
	if (!getMissileAtTop()) {
		moveUpMissileVAO();
	} else if (!getMissileExploded()) {
		moveDownMissileVAO();
	}

	//Increase the size of the explosion cloud over time
	if (getMissileExploded() && getCurMissileExpSize() < 1.0f) {
		setMissileExpScale(0.008f);
	}

	//changes the scale of the missile smoke over time
	switch (getSmokeScaleDir()) {
		case true: setSmokeScaleY(0.005f); break;
		case false: setSmokeScaleY(-0.005f); break;
	}

	//moves the cloud over time
	switch (getCloudDir()) {
		case true: setCloudX(cloudDeltaX); break;
		case false: setCloudX(cloudDeltaX * -1); break;
	}

	// Redraw the screen
	glutPostRedisplay();
}

#pragma region event handling
void keyDown(unsigned char key, int x, int y) {
	//check if the missile has already exploded
	if (!getMissileExploded()) {
		std::cout << key << " pressed\n";

		switch (tolower(key)) {
			case 'a': setMissileX(-0.02f); break;
			case 'd': setMissileX(0.02f); break;
		}

		glutPostRedisplay();
	}
}

void mouseButtonDown(int button_id, int state, int x, int y) {
	//If the mouse button is down then increase the x speed of the cloud
	if (button_id == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			cloudDeltaX = 0.05f;

		} else if (state == GLUT_UP) {
			cloudDeltaX = 0.003f;
		}
	}
}
#pragma endregion event handling