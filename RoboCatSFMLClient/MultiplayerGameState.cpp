#include "RoboCatClientPCH.hpp"

MultiplayerGameState::MultiplayerGameState()
	:m_world_client(dynamic_cast<WorldClient*>(World::sInstance.get()))
{
	//Load the Level
	m_world_client->LoadLevel();
	m_world_client->SetCamera(WindowManager::sInstance->getDefaultView());
}

void MultiplayerGameState::OnStackPopped()
{
	m_world_client->RemoveAllGameObjects();
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
	//Draw the Level using the worlds camera (the client player's camera)
	RenderManager::sInstance->RenderComponents(m_world_client->GetCamera());
}

