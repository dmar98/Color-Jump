#include "RoboCatClientPCH.hpp"

TutorialState::TutorialState(StateStack& stack) : State(stack)
{
}

void TutorialState::Draw()
{
}

bool TutorialState::Update(float dt)
{
	return true;
}

bool TutorialState::HandleEvent(const sf::Event& event)
{
	return false;
}
