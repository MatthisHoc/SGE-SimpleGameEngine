#include "game.h"

#include "core/Window.h"

int run()
{
	sg::Window window("Zizi", 0, 0, 1000, 750);

	while(window.processEvents())
	{
		window.draw();
	}
	
	return 0;
}