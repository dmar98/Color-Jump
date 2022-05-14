#include "ColorJumpClientPCH.hpp"

LevelPauseState::LevelPauseState(): State()
{
}

void LevelPauseState::Draw()
{
}

bool LevelPauseState::Update(float dt)
{
	return false;
}

bool LevelPauseState::HandleEvent(const sf::Event& event)
{
	return false;
}
