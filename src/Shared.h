#ifndef SHARED_H
#define SHARED_H

#include "nexus/Nexus.h"
#include "mumble/Mumble.h"

extern HMODULE SelfModule;
extern AddonAPI* APIDefs;

extern HWND GameHandle;
extern Mumble::Data* MumbleLink;

extern Texture* Alacrity;
extern Texture* Fury;
extern Texture* Heal;
extern Texture* Might;
extern Texture* Quickness;
extern Texture* Vulnerability;

extern Texture* MW_Grid;
extern Texture* MW_Arrow;
extern Texture* MW_ArrowHover;
extern Texture* MW_Circle;
extern Texture* MW_CircleHover;
extern Texture* MW_Heart;
extern Texture* MW_HeartHover;
extern Texture* MW_Square;
extern Texture* MW_SquareHover;
extern Texture* MW_Star;
extern Texture* MW_StarHover;
extern Texture* MW_Swirl;
extern Texture* MW_SwirlHover;
extern Texture* MW_Triangle;
extern Texture* MW_TriangleHover;
extern Texture* MW_Cross;
extern Texture* MW_CrossHover;

#endif