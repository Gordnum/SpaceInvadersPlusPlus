#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include "Utils.h"

class Cutscene
{
	private:
        SDL_Renderer* renderer;
        TTF_Font* font;

        std::vector<std::string> lines;

        int currentLine;
        bool active;
        bool finished;

        unsigned int lastSwitchTime;
        unsigned int delayPerLine;

	public:
        Cutscene(SDL_Renderer* r);
        void start(const std::vector<std::string>& lines);
        void update();
        void render();
        bool isActive() const;
        bool isFinished() const;

};