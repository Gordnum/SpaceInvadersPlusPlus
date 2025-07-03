#include <SDL.h>
#include "Game.h"
#include <iostream>

int main(int argc, char *argv[])
{
	// testing
	Game game;
	if(game.init())
	{
		game.run();
	}
	return 0;
}