#define SDL_MAIN_HANDLED 

#include <exception>
#include <iostream>

extern int run();

int main()
{
	try
	{
		return run();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return -1;
}