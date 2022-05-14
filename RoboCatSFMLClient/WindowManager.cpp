#include "ColorJumpClientPCH.hpp"

std::unique_ptr<sf::RenderWindow> WindowManager::sInstance;

bool WindowManager::StaticInit()
{
	sInstance.reset(new sf::RenderWindow(sf::VideoMode(1344, 960), "Color War", sf::Style::Close));
	return true;
}
