#include "RoboCatClientPCH.hpp"

LevelLoseState::LevelLoseState(StateStack& stack): State(stack)
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
