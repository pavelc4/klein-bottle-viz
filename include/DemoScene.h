#ifndef DEMO_SCENE_H
#define DEMO_SCENE_H

#include "KleinMath.h"

void DemoScene_Update(float dt);
void DemoScene_Render(float bottleRadius);
void DemoScene_RenderTrail();
void DemoScene_RenderSecond(float bottleRadius);

void DemoScene_Toggle();
void DemoScene_Reset();

void  DemoScene_SetSpeed(float s);
float DemoScene_GetSpeed();
void  DemoScene_SetTrailVisible(bool v);
bool  DemoScene_IsTrailVisible();
bool  DemoScene_IsActive();

void  DemoScene_GetUV(float& u, float& v);
void  DemoScene_GetUV2(float& u, float& v);

#endif
