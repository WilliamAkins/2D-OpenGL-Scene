#include <stack>
#include "stdafx.h"
#include "draw_scene.h"

using namespace CoreStructures;

//Define pi for use with angles
static const float PI = 3.14159;

//location of "T" uniform variables for each shader
GLuint locT, locT2;

//Shader program objects for applying shaders to shapes
GLuint myShaderProgram;
GLuint myShaderProgramNoTexture;

//Textures
GLuint skyTexture = 0;
GLuint groundTexture = 1;
GLuint grassTexture = 2;
GLuint explosionTexture = 3;
GLuint cloudTexture = 4;

//Creates the VAOs
GLuint skyVAO, groundVAO, GrassVAO, missileBodyVAO, missileThrusterVAO, missileSmokeVAO, missileExplosionVAO, cloudVAO;

//matrix stack used to store transformation matrices for hierarchical model (this allows the user to move back and forth around a complex hierarchical model more easily) 
std::stack<GUMatrix4> matrixStack;

void setupTextures(void) {
	skyTexture = fiLoadTexture("Assets\\sky.jpg");
	groundTexture = fiLoadTexture("Assets\\ground.jpg");
	grassTexture = fiLoadTexture("Assets\\grass.jpg");
	explosionTexture = fiLoadTexture("Assets\\explosion.jpg");
	cloudTexture = fiLoadTexture("Assets\\cloud.jpg");
}

void setupShaders(void) {
	// Shader setup 
	myShaderProgram = setupShaders(std::string("Shaders\\basic_vert.glsl"), std::string("Shaders\\basic_frag.glsl"));
	myShaderProgramNoTexture = setupShaders(std::string("Shaders\\notexture_vert.glsl"), std::string("Shaders\\notexture_frag.glsl"));

	// Get uniform location of "T" variable in shader program (we'll use this in the play function to give the uniform variable "T" a value)
	locT = glGetUniformLocation(myShaderProgram, "T");
	locT2 = glGetUniformLocation(myShaderProgramNoTexture, "T2");
}

#pragma region ground object
void setupSkyVAO(void) {
	// Vertex Buffer Object IDs for the ground texture object
	GLuint quadPosVBO, quadColourVBO, quadTexCoordVBO, quadIndicesVBO;

	// 1) Position Array - Store vertices as (x,y) pairs
	static GLfloat quadVertices[] = {
		-1.0, -1.0f,
		-1.0f, 1.0f,
		1.0f, -1.0f,
		1.0f, 1.0f
	};

	// 2) Colour Array - Store RGB values as unsigned bytes
	static GLubyte quadColors[] = {
		255, 0, 0, 255,
		255, 255, 0, 255,
		0, 255, 0, 255,
		0, 255, 255, 255
	};

	// 3) Texture coordinate array (store uv coordinates as floating point values)
	static float quadTextureCoords[] = {
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	// 4) Index Array - Store indices to quad vertices - this determines the order the vertices are to be processed
	static GLubyte quadVertexIndices[] = { 0, 1, 2, 3 };

	//create and bind the VAO
	glGenVertexArrays(1, &skyVAO);
	glBindVertexArray(skyVAO);

	// setup VBO for the quad object position data
	glGenBuffers(1, &quadPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	// setup VBO for the quad object colour data
	glGenBuffers(1, &quadColourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadColourVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadColors), quadColors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(1);

	// setup VBO for the quad object texture coord data
	glGenBuffers(1, &quadTexCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadTexCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadTextureCoords), quadTextureCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(2);

	// setup quad vertex index array
	glGenBuffers(1, &quadIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadVertexIndices), quadVertexIndices, GL_STATIC_DRAW);

	//Unbind the VAO once created
	glBindVertexArray(0);
}

void drawSkyVAO(void) {
	//Pass shader program into GPU pipeline
	glUseProgram(myShaderProgram);

	// Move our ground shape to the top half of the screen
	GUMatrix4 T = GUMatrix4::translationMatrix(0.0f, 0.0f, 0.0f);
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&T);

	//bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, skyTexture);
	glUniform1i(glGetUniformLocation(myShaderProgram, "texture"), 0);
	glEnable(GL_TEXTURE_2D);

	//Enables blending to support alpha
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	//bind ground VAO and draw it
	glBindVertexArray(skyVAO);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid*)0);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glUseProgram(0);
}
#pragma endregion sky object

#pragma region ground object
void setupGroundVAO(void) {
	// Vertex Buffer Object IDs for the ground texture object
	GLuint quadPosVBO, quadColourVBO, quadTexCoordVBO, quadIndicesVBO;

	// 1) Position Array - Store vertices as (x,y) pairs
	static GLfloat quadVertices[] = {
		-1.0, -0.5f,
		-1.0f, 0.3f,
		1.0f, -0.5f,
		1.0f, 0.3f
	};

	// 2) Colour Array - Store RGB values as unsigned bytes
	static GLubyte quadColors[] = {
		255, 0, 0, 255,
		255, 255, 0, 255,
		0, 255, 0, 255,
		0, 255, 255, 255
	};

	// 3) Texture coordinate array (store uv coordinates as floating point values)
	static float quadTextureCoords[] = {
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	// 4) Index Array - Store indices to quad vertices - this determines the order the vertices are to be processed
	static GLubyte quadVertexIndices[] = { 0, 1, 2, 3 };

	//create and bind the VAO
	glGenVertexArrays(1, &groundVAO);
	glBindVertexArray(groundVAO);

	// setup VBO for the quad object position data
	glGenBuffers(1, &quadPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	// setup VBO for the quad object colour data
	glGenBuffers(1, &quadColourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadColourVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadColors), quadColors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(1);

	// setup VBO for the quad object texture coord data
	glGenBuffers(1, &quadTexCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadTexCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadTextureCoords), quadTextureCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(2);

	// setup quad vertex index array
	glGenBuffers(1, &quadIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadVertexIndices), quadVertexIndices, GL_STATIC_DRAW);

	//Unbind the VAO once created
	glBindVertexArray(0);
}

void drawGroundVAO(void) {
	//Pass shader program into GPU pipeline
	glUseProgram(myShaderProgram);

	// Move our ground shape to the bottom half of the screen
	GUMatrix4 T = GUMatrix4::translationMatrix(0.0f, -0.5f, 0.0f);
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&T);

	//bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, groundTexture);
	glUniform1i(glGetUniformLocation(myShaderProgram, "texture"), 0);
	glEnable(GL_TEXTURE_2D);

	//Enables blending to support alpha
	glEnable(GL_BLEND);
	glBlendEquation(GL_MAX);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);

	//bind ground VAO and draw it
	glBindVertexArray(groundVAO);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid*)0);

	glDisable(GL_BLEND);
	glUseProgram(0);
}
#pragma endregion ground object

#pragma region grass object
void setupGrassVAO(void) {
	// Vertex Buffer Object IDs for the grass texture object
	GLuint grassPosVBO, grassColourVBO, grassTexCoordVBO, grassIndicesVBO;

	// 1) Position Array - Store vertices as (x,y) pairs
	static GLfloat grassVertices[] = {
		-1.0, -0.2f,
		-1.0f, 0.2f,
		1.0f, -0.2f,
		1.0f, 0.2f
	};

	// 2) Colour Array - Store RGB values as unsigned bytes
	static GLubyte grassColors[] = {
		255, 0, 0, 255,
		255, 255, 0, 255,
		0, 255, 0, 255,
		0, 255, 255, 255
	};

	// 3) Texture coordinate array (store uv coordinates as floating point values)
	static float grassTextureCoords[] = {
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	// 4) Index Array - Store indices to quad vertices - this determines the order the vertices are to be processed
	static GLubyte grassVertexIndices[] = { 0, 1, 2, 3 };

	//create and bind the VAO
	glGenVertexArrays(1, &GrassVAO);
	glBindVertexArray(GrassVAO);

	// setup VBO for the quad object position data
	glGenBuffers(1, &grassPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, grassPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), grassVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	// setup VBO for the quad object colour data
	glGenBuffers(1, &grassColourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, grassColourVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassColors), grassColors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(1);

	// setup VBO for the quad object texture coord data
	glGenBuffers(1, &grassTexCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, grassTexCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassTextureCoords), grassTextureCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(2);

	// setup quad vertex index array
	glGenBuffers(1, &grassIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grassIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(grassVertexIndices), grassVertexIndices, GL_STATIC_DRAW);

	//Unbind the VAO once created
	glBindVertexArray(0);
}

void drawGrassVAO(void) {
	//Pass shader program into GPU pipeline
	glUseProgram(myShaderProgram);

	// Move the grass shape to the bottom half of the screen
	GUMatrix4 T = GUMatrix4::translationMatrix(0.0f, -0.8f, 0.0f);
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&T);

	//bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grassTexture);
	glUniform1i(glGetUniformLocation(myShaderProgram, "texture"), 0);
	glEnable(GL_TEXTURE_2D);

	//Enables blending to support alpha
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
	//glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE_MINUS_SRC_ALPHA);


	//bind grass VAO and draw it
	glBindVertexArray(GrassVAO);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid*)0);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glUseProgram(0);
}
#pragma endregion grass object

#pragma region missile object
//Setup a global struct for the missile object
struct missileAttrib {
	bool atTop;
	bool exploded;
	float x = 0.0f;
	float y = -0.5f;
	float theta = 0.0f;
	float scale = 1.0f;
	float smokeScaleY = 1.4f;
	float smokeOffsetY = -0.24f;
	bool smokeScaleUp = true;
} missile;

void setupMissileVAO(void) {
	//Draws the main missile object, the body
	setupMissileBodyVAO();

	setupMissileThrusterVAO();
	setupMissileSmokeVAO();
}

void setupMissileBodyVAO(void) {
	// Vertex Buffer Object IDs for the missile 
	GLuint missilePosVBO, missileColourVBO, missileTexCoordVBO, missileIndicesVBO;

	// 1) Position Array - Store vertices as (x,y) pairs
	static GLfloat missileVertices[] = {
		0.0f, 0.5f,
		-0.2f, 0.2f,
		0.2f, 0.2f,
		-0.2f, -0.4f,
		0.2f, -0.4f,
	};

	// 2) Colour Array - Store RGB values as unsigned bytes
	static GLubyte missileColors[] = {
		50, 50, 50, 255,
		200, 200, 200, 255,
		200, 200, 200, 255,
		200, 200, 200, 255,
		200, 200, 200, 255
	};

	// 3) Texture coordinate array (store uv coordinates as floating point values)
	static float missileTextureCoords[] = {
		0.0f, 0.5f,
		-0.2f, 0.2f,
		0.2f, 0.2f,
		-0.2f, -0.4f,
		0.2f, -0.4f,
	};

	// 4) Index Array - Store indices to missile vertices - this determines the order the vertices are to be processed
	static GLubyte missileVertexIndices[] = { 0, 1, 2, 3, 4 };

	//create and bind the VAO
	glGenVertexArrays(1, &missileBodyVAO);
	glBindVertexArray(missileBodyVAO);

	// setup VBO for the missile object position data
	glGenBuffers(1, &missilePosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, missilePosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(missileVertices), missileVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	// setup VBO for the missile object colour data
	glGenBuffers(1, &missileColourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, missileColourVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(missileColors), missileColors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(1);

	// setup VBO for the missile object texture coord data
	glGenBuffers(1, &missileTexCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, missileTexCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(missileTextureCoords), missileTextureCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(2);

	// setup missile vertex index array
	glGenBuffers(1, &missileIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, missileIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(missileVertexIndices), missileVertexIndices, GL_STATIC_DRAW);

	//Unbind the VAO once created
	glBindVertexArray(0);
}

void setupMissileThrusterVAO(void) {
	// Vertex Buffer Object IDs for the missile 
	GLuint missileThrusterPosVBO, missileThrusterColourVBO, missileThrusterTexCoordVBO, missileThrusterIndicesVBO;

	// 1) Position Array - Store vertices as (x,y) pairs
	static GLfloat missileThrusterVertices[] = {
		-0.05f, 0.1f,
		0.05f, 0.1f,
		-0.1f, -0.1f,
		0.1f, -0.1f,
	};

	// 2) Colour Array - Store RGB values as unsigned bytes
	static GLubyte missileThrusterColors[] = {
		170, 170, 170, 255,
		170, 170, 170, 255,
		90, 90, 90, 255,
		90, 90, 90, 255
	};

	// 3) Texture coordinate array (store uv coordinates as floating point values)
	static float missileThrusterTextureCoords[] = {
		-0.05f, 0.1f,
		0.05f, 0.1f,
		-0.1f, -0.1f,
		0.1f, -0.1f,
	};

	// 4) Index Array - Store indices to missile vertices - this determines the order the vertices are to be processed
	static GLubyte missileThrusterVertexIndices[] = { 0, 1, 2, 3 };

	//create and bind the VAO
	glGenVertexArrays(1, &missileThrusterVAO);
	glBindVertexArray(missileThrusterVAO);

	// setup VBO for the missile object position data
	glGenBuffers(1, &missileThrusterPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, missileThrusterPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(missileThrusterVertices), missileThrusterVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	// setup VBO for the missile object colour data
	glGenBuffers(1, &missileThrusterColourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, missileThrusterColourVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(missileThrusterColors), missileThrusterColors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(1);

	// setup VBO for the missile object texture coord data
	glGenBuffers(1, &missileThrusterTexCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, missileThrusterTexCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(missileThrusterTextureCoords), missileThrusterTextureCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(2);

	// setup missile vertex index array
	glGenBuffers(1, &missileThrusterIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, missileThrusterIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(missileThrusterVertexIndices), missileThrusterVertexIndices, GL_STATIC_DRAW);

	//Unbind the VAO once created
	glBindVertexArray(0);
}

void setupMissileSmokeVAO(void) {
	// Vertex Buffer Object IDs for the smoke 
	GLuint missileSmokePosVBO, missileSmokeColourVBO, missileSmokeTexCoordVBO, missileSmokeIndicesVBO;

	// 1) Position Array - Store vertices as (x,y) pairs
	static GLfloat missileSmokeVertices[] = {
		0.05f, 0.1f,
		-0.05f, 0.1f,
		0.15f, -0.05f,
		-0.15f, -0.05f,
		0.0f, -0.15f
	};

	// 2) Colour Array - Store RGB values as unsigned bytes
	static GLubyte missileSmokeColors[] = {
		138, 39, 9, 255,
		138, 39, 9, 255,
		198, 105, 14, 255,
		198, 105, 14, 255,
		158, 0, 0, 255
	};

	// 3) Texture coordinate array (store uv coordinates as floating point values)
	static float missileSmokeTextureCoords[] = {
		0.05f, 0.1f,
		-0.05f, 0.1f,
		0.15f, -0.05f,
		-0.15f, -0.05f,
		0.0f, -0.15f
	};

	// 4) Index Array - Store indices to missile vertices - this determines the order the vertices are to be processed
	static GLubyte missileSmokeVertexIndices[] = { 0, 1, 2, 3, 4 };

	//create and bind the VAO
	glGenVertexArrays(1, &missileSmokeVAO);
	glBindVertexArray(missileSmokeVAO);

	// setup VBO for the missile object position data
	glGenBuffers(1, &missileSmokePosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, missileSmokePosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(missileSmokeVertices), missileSmokeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	// setup VBO for the missile object colour data
	glGenBuffers(1, &missileSmokeColourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, missileSmokeColourVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(missileSmokeColors), missileSmokeColors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(1);

	// setup VBO for the missile object texture coord data
	glGenBuffers(1, &missileSmokeTexCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, missileSmokeTexCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(missileSmokeTextureCoords), missileSmokeTextureCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(2);

	// setup missile vertex index array
	glGenBuffers(1, &missileSmokeIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, missileSmokeIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(missileSmokeVertexIndices), missileSmokeVertexIndices, GL_STATIC_DRAW);

	//Unbind the VAO once created
	glBindVertexArray(0);
}

void drawMissileVAO(void) {
	//Pass shader program into GPU pipeline
	glUseProgram(myShaderProgramNoTexture);

	//setup matrix as an identity matrix
	GUMatrix4 M = GUMatrix4::identity();

	matrixStack.push(M);

	//Transform the base object, the missile body
	GUMatrix4 T = GUMatrix4::translationMatrix(missile.x, missile.y, 0.0f);
	GUMatrix4 R = GUMatrix4::rotationMatrix(0.0f, 0.0f, missile.theta*(PI / 180));
	GUMatrix4 S = GUMatrix4::scaleMatrix(missile.scale, missile.scale, 0.0f);
	M = T * R * S;
	glUniformMatrix4fv(locT2, 1, GL_FALSE, (GLfloat*)&M);

	drawMissileBodyVAO();

	matrixStack.push(M);

	//transform the left missile thruster
	M = M * GUMatrix4::translationMatrix(-0.13f, -0.5f, 0.0f);
	glUniformMatrix4fv(locT2, 1, GL_FALSE, (GLfloat*)&M);

	drawMissileThrusterVAO();

	matrixStack.push(M);

	//transform the left missile smoke
	M = M * GUMatrix4::translationMatrix(0.0f, missile.smokeOffsetY, 0.0f) * GUMatrix4::scaleMatrix(1.0f, missile.smokeScaleY, 0.0f);
	glUniformMatrix4fv(locT2, 1, GL_FALSE, (GLfloat*)&M);

	drawMissileSmokeVAO();

	M = matrixStack.top();
	matrixStack.pop();

	M = matrixStack.top();
	matrixStack.pop();

	//transform the right missile thruster
	M = M * GUMatrix4::translationMatrix(0.13f, -0.5f, 0.0f);
	glUniformMatrix4fv(locT2, 1, GL_FALSE, (GLfloat*)&M);

	drawMissileThrusterVAO();

	matrixStack.push(M);

	//transform the right missile smoke
	M = M * GUMatrix4::translationMatrix(0.0f, missile.smokeOffsetY, 0.0f) * GUMatrix4::scaleMatrix(1.0f, missile.smokeScaleY, 0.0f);
	glUniformMatrix4fv(locT2, 1, GL_FALSE, (GLfloat*)&M);

	drawMissileSmokeVAO();

	M = matrixStack.top();
	matrixStack.pop();

	M = matrixStack.top();
	matrixStack.pop();

	glUseProgram(0);
}

void drawMissileBodyVAO(void) {
	//bind missile body VAO and draw it
	glBindVertexArray(missileBodyVAO);
	glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_BYTE, (GLvoid*)0);
}

void drawMissileThrusterVAO(void) {
	//bind missile thruster VAO and draw it
	glBindVertexArray(missileThrusterVAO);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid*)0);
}

void drawMissileSmokeVAO(void) {
	//bind missile thruster VAO and draw it
	glBindVertexArray(missileSmokeVAO);
	glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_BYTE, (GLvoid*)0);
}

void moveUpMissileVAO(void) {
	if (missile.y < 1.9f) {
		missile.y += 0.004f;
	} else {
		missile.atTop = true;
		missile.scale = 0.5f;
		missile.theta = 180.0f;
		std::cout << "Missile has reached the top... It's coming down!\n";
	}
}

void moveDownMissileVAO(void) {
	if (missile.y > -0.3) {
		missile.y -= 0.004f;
	} else {
		missile.exploded = true;
		std::cout << "Missile has reached the bottom... It's going to blow up!\n";

		//Reduces the size of the missile to 0 so it appears invisible
		missile.scale = 0.0f;
	}
}

void setMissileX(float deltaX) {
	missile.x += deltaX;
}

void setSmokeScaleY(float deltaScale) {
	static float offsetY;
	//reverses the direction of the smoke scale onces it reaches the edges
	if (missile.smokeScaleY > 1.4f && missile.smokeScaleUp == true) {
		missile.smokeScaleUp = false;
		offsetY = 0.0005f;
	}
	else if (missile.smokeScaleY < 1.0f && missile.smokeScaleUp == false) {
		missile.smokeScaleUp = true;
		offsetY = -0.0005f;
	}

	missile.smokeOffsetY += offsetY;
	missile.smokeScaleY += deltaScale;
}

bool getSmokeScaleDir(void) {
	return missile.smokeScaleUp;
}

bool getMissileAtTop(void) {
	return missile.atTop;
}

bool getMissileExploded(void) {
	return missile.exploded;
}
#pragma endregion missile object

#pragma region missile explosion object
struct missileExpAttrib {
	float scale = 0.0f;
	float x = 0.0f;
	float y = -0.3f;
} missileExp;

void setupMissileExplosionVAO(void) {
	// Vertex Buffer Object IDs for the ground texture object
	GLuint quadPosVBO, quadColourVBO, quadTexCoordVBO, quadIndicesVBO;

	// 1) Position Array - Store vertices as (x,y) pairs
	static GLfloat quadVertices[] = {
		-1.0, -0.7f,
		-1.0f, 0.7f,
		1.0f, -0.7f,
		1.0f, 0.7f
	};

	// 2) Colour Array - Store RGB values as unsigned bytes
	static GLubyte quadColors[] = {
		255, 0, 0, 255,
		255, 255, 0, 255,
		0, 255, 0, 255,
		0, 255, 255, 255
	};

	// 3) Texture coordinate array (store uv coordinates as floating point values)
	static float quadTextureCoords[] = {
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	// 4) Index Array - Store indices to quad vertices - this determines the order the vertices are to be processed
	static GLubyte quadVertexIndices[] = { 0, 1, 2, 3 };

	//create and bind the VAO
	glGenVertexArrays(1, &missileExplosionVAO);
	glBindVertexArray(missileExplosionVAO);

	// setup VBO for the quad object position data
	glGenBuffers(1, &quadPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	// setup VBO for the quad object colour data
	glGenBuffers(1, &quadColourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadColourVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadColors), quadColors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(1);

	// setup VBO for the quad object texture coord data
	glGenBuffers(1, &quadTexCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadTexCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadTextureCoords), quadTextureCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(2);

	// setup quad vertex index array
	glGenBuffers(1, &quadIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadVertexIndices), quadVertexIndices, GL_STATIC_DRAW);

	//Unbind the VAO once created
	glBindVertexArray(0);
}

void drawMissileExplosionVAO(void) {
	//Pass shader program into GPU pipeline
	glUseProgram(myShaderProgram);

	// Move our ground shape to the top half of the screen
	GUMatrix4 T = GUMatrix4::translationMatrix(missile.x, missileExp.y, 0.0f);
	GUMatrix4 S = GUMatrix4::scaleMatrix(missileExp.scale, missileExp.scale, 0.0f);
	GUMatrix4 M = T * S;
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&M);

	//bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, explosionTexture);
	glUniform1i(glGetUniformLocation(myShaderProgram, "texture"), 0);
	glEnable(GL_TEXTURE_2D);

	//Enables blending to support alpha
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	//bind ground VAO and draw it
	glBindVertexArray(missileExplosionVAO);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid*)0);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glUseProgram(0);
}

void setMissileExpScale(float deltaScale) {
	missileExp.scale += deltaScale;

	//modify the y position of the explosion to grow from a certain point as it scales
	missileExp.y += 0.004f;
}

float getCurMissileExpSize(void) {
	return missileExp.scale;
}
#pragma endregion missile explosion object

#pragma region cloud object
struct cloudAttrib {
	float x;
	bool moveRight = true;
} cloud;

void setupCloudVAO(void) {
	// Vertex Buffer Object IDs for the grass texture object
	GLuint cloudPosVBO, cloudColourVBO, cloudTexCoordVBO, cloudIndicesVBO;

	// 1) Position Array - Store vertices as (x,y) pairs
	static GLfloat cloudVertices[] = {
		-0.3f, -0.3f,
		-0.3f, 0.0f,
		0.3f, -0.3f,
		0.3f, 0.0f
	};

	// 2) Colour Array - Store RGB values as unsigned bytes
	static GLubyte cloudColors[] = {
		255, 0, 0, 255,
		255, 255, 0, 255,
		0, 255, 0, 255,
		0, 255, 255, 255
	};

	// 3) Texture coordinate array (store uv coordinates as floating point values)
	static float cloudTextureCoords[] = {
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	// 4) Index Array - Store indices to quad vertices - this determines the order the vertices are to be processed
	static GLubyte cloudVertexIndices[] = { 0, 1, 2, 3 };

	//create and bind the VAO
	glGenVertexArrays(1, &cloudVAO);
	glBindVertexArray(cloudVAO);

	// setup VBO for the quad object position data
	glGenBuffers(1, &cloudPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cloudPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cloudVertices), cloudVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	// setup VBO for the quad object colour data
	glGenBuffers(1, &cloudColourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cloudColourVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cloudColors), cloudColors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(1);

	// setup VBO for the quad object texture coord data
	glGenBuffers(1, &cloudTexCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cloudTexCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cloudTextureCoords), cloudTextureCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(2);

	// setup quad vertex index array
	glGenBuffers(1, &cloudIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cloudIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cloudVertexIndices), cloudVertexIndices, GL_STATIC_DRAW);

	//Unbind the VAO once created
	glBindVertexArray(0);
}

void drawCloudVAO(void) {
	//Pass shader program into GPU pipeline
	glUseProgram(myShaderProgram);

	// Move the grass shape to the bottom half of the screen
	GUMatrix4 T = GUMatrix4::translationMatrix(cloud.x, 0.3f, 0.0f);
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&T);

	//bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cloudTexture);
	glUniform1i(glGetUniformLocation(myShaderProgram, "texture"), 0);
	glEnable(GL_TEXTURE_2D);

	//Enables blending to support alpha
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);


	//bind grass VAO and draw it
	glBindVertexArray(cloudVAO);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid*)0);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glUseProgram(0);
}

void setCloudX(float deltaX) {
	//reverses the direction of the cloud onces it reaches the edges
	if (cloud.x > 1.4f && cloud.moveRight == true) {
		cloud.moveRight = false;
	} else if (cloud.x < -1.4f && cloud.moveRight == false) {
		cloud.moveRight = true;
	}

	cloud.x += deltaX;
}

bool getCloudDir(void) {
	return cloud.moveRight;
}
#pragma endregion cloud object