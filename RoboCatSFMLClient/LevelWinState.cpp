#include "RoboCatClientPCH.hpp"


LevelWinState::LevelWinState(): State()
{
}

void LevelWinState::Draw()
{
}

bool LevelWinState::Update(float dt)
{
	return false;
}

bool LevelWinState::HandleEvent(const sf::Event& event)
{
	return false;
}
