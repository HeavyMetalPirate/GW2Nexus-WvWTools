#ifndef ADDON_RENDERER_H
#define ADDON_RENDERER_H

#include "../Globals.h"
#include "GuildInfoService.h"

class Renderer {
public:
	void preRender();
	void render();
	void postRender();

	void unload();
private: 
};

#endif