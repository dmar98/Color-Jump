#include "RoboCatClientPCH.hpp"


LevelWinState::LevelWinState(StateStack& stack): State(stack)
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
