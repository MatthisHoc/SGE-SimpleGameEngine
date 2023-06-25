#include "game.h"

#include "core/Window.h"
#include "core/Object/Object.h"
#include "core/Game.h"

#include "scripts/TestScript/TestScript.h"

int run()
{
	sg::Window window("Hello Monkey", 0, 0, 1000, 750);

	sg::Game::instanciate("monkey.sgo");

	while(window.processEvents())
	{
		window.draw();
	}
	
	return 0;
}