#include "RoboCatClientPCH.hpp"

LevelPauseState::LevelPauseState(StateStack& stack): State(stack)
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
