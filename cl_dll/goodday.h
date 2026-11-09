#pragma once

#ifndef GOOD_DAY_H
#define GOOD_DAY_H
#include "hud.h"
#include "cl_util.h"
#include "cl_entity.h"
#include "triangleapi.h"
#include "cdll_int.h"
#include "pl_mpeg.h"
#include "glad.h"

extern qboolean IsGoodDay;
extern qboolean GLLoaded;
extern float x;
extern float y;
extern float z;
extern int w;
extern int h;
extern double fps;
extern uint8_t* pixels;
extern plm_frame_t* frame;
extern plm_t* plm;
extern double current_time;
extern double elapsed_time;
extern double last_time;

extern unsigned int texture;

void RenderGoodDay();

#endif