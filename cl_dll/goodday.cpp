#include "goodday.h"

float x = 0;
float y = 0;
float z = 0;
double fps = 0;

double current_time = 0;
double elapsed_time = 0;
double last_time = 0;

double accumulator = 0;

void RenderGoodDay()
{
	if (!IsGoodDay || !plm || !GLLoaded)
	{
		return;
	}

	vec3_t org;

	org = { x, y, z };

	current_time = (double)gEngfuncs.GetClientTime();
	elapsed_time = current_time - last_time;
	if (elapsed_time > 1.0 / fps) {
		elapsed_time = 1.0 / fps;
	}
	last_time = current_time;
	
	accumulator += elapsed_time;

	glPushAttrib(GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture);
	
	while (accumulator >= 1.0 / fps)
	{
		//gEngfuncs.Con_Printf("start decode...\n");
		frame = plm_decode_video(plm);
		if (frame)
		{
			//gEngfuncs.Con_Printf("Frame!\n", fps, elapsed_time);

			pixels = (uint8_t*)malloc(w * h * 3);
			plm_frame_to_rgb(frame, pixels, frame->width * 3);

			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

			free(pixels);

			//gEngfuncs.Con_Printf("decoded!\n");
		}
		else
		{
			//gEngfuncs.Con_Printf("fail :(\n");
		}
		accumulator -= 1.0 / fps;
	}

	//gEngfuncs.Con_Printf("FPS: %f, Elapsed time: %f, Next rendertime: %f, Curtime: %f\n", fps, elapsed_time, nexttime, current_time);

	// Create a triangle, sigh
	gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
	gEngfuncs.pTriAPI->CullFace(TRI_NONE);
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	// Overload p->color with index into tracer palette, p->packedColor with brightness
	gEngfuncs.pTriAPI->Color4f(1.0, 1.0, 1.0, 1.0);
	// UNDONE: This gouraud shading causes tracers to disappear on some cards (permedia2)
	gEngfuncs.pTriAPI->Brightness(1);
	gEngfuncs.pTriAPI->TexCoord2f(0, 1);
	gEngfuncs.pTriAPI->Vertex3f(org.x, org.y, org.z);

	gEngfuncs.pTriAPI->Brightness(1);
	gEngfuncs.pTriAPI->TexCoord2f(0, 0);
	gEngfuncs.pTriAPI->Vertex3f(org.x, org.y, org.z + 192);

	gEngfuncs.pTriAPI->Brightness(1);
	gEngfuncs.pTriAPI->TexCoord2f(1, 0);
	gEngfuncs.pTriAPI->Vertex3f(org.x, org.y - 256, org.z + 192);

	gEngfuncs.pTriAPI->Brightness(1);
	gEngfuncs.pTriAPI->TexCoord2f(1, 1);
	gEngfuncs.pTriAPI->Vertex3f(org.x, org.y - 256, org.z);

	gEngfuncs.pTriAPI->End();
	gEngfuncs.pTriAPI->RenderMode(kRenderNormal);

	glPopAttrib();
}