#include "RoboCatClientPCH.hpp"

LevelLoseState::LevelLoseState(): State()
{
}

void LevelLoseState::Draw()
{
}

bool LevelLoseState::Update(float dt)
{
	return false;
}

bool LevelLoseState::HandleEvent(const sf::Event& event)
{
	return false;
}
