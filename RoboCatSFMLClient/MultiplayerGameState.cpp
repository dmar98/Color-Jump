#include "RoboCatClientPCH.hpp"

MultiplayerGameState::MultiplayerGameState()
{
	//Load the Level
	World::sInstance->LoadLevel();
}

MultiplayerGameState::~MultiplayerGameState()
{
	World::sInstance->DisposeLevel();
}

bool MultiplayerGameState::Update(float dt)
{
	return false;
}

bool MultiplayerGameState::HandleEvent(const sf::Event& event)
{
	return false;
}

void MultiplayerGameState::Draw()
{
	//Draw the Level
	RenderManager::sInstance->RenderComponents();
}

