#pragma once

void setupTextures(void);
void setupShaders(void);

void setupSkyVAO(void);
void drawSkyVAO(void);

void setupGroundVAO(void);
void drawGroundVAO(void);

void setupGrassVAO(void);
void drawGrassVAO(void);

//Function prototypes for the missile
void setupMissileVAO(void);
void setupMissileBodyVAO(void);
void setupMissileThrusterVAO(void);
void setupMissileSmokeVAO(void);
void drawMissileVAO(void);
void drawMissileBodyVAO(void);
void drawMissileThrusterVAO(void);
void drawMissileSmokeVAO(void);
void moveUpMissileVAO(void);
void moveDownMissileVAO(void);
void setMissileX(float);
void setSmokeScaleY(float);
bool getSmokeScaleDir(void);
bool getMissileAtTop(void);
bool getMissileExploded(void);

//Function prototypes for the missile explosion
void setupMissileExplosionVAO(void);
void drawMissileExplosionVAO(void);
void setMissileExpScale(float);
float getCurMissileExpSize(void);

//function prototypes for the clouds
void setupCloudVAO(void);
void drawCloudVAO(void);
void setCloudX(float);
bool getCloudDir();