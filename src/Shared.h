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

extern Texture* SQM_1_ARROW;
extern Texture* SQM_2_CIRCLE;
extern Texture* SQM_3_HEART;
extern Texture* SQM_4_SQUARE;
extern Texture* SQM_5_STAR;
extern Texture* SQM_6_SWIRL;
extern Texture* SQM_7_TRIANGLE;
extern Texture* SQM_8_CROSS;

#endif
