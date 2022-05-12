#include "RoboCatClientPCH.hpp"

bool Client::StaticInit()
{
	// Create the Client pointer first because it initializes SDL
	auto* client = new Client();
	InputManager::StaticInit();
	KeyBinding::StaticInit();
	WindowManager::StaticInit();
	FontManager::StaticInit();
	TextureManager::StaticInit();
	AudioManager::StaticInit();
	RenderManager::StaticInit();
	StackManager::StaticInit();
	PlayerDataManager::StaticInit();
	HUD::StaticInit();
	World::StaticInit(new WorldClient());

	s_instance.reset(client);

	return true;
}

Client::Client() = default;

void Client::DoFrame()
{
	InputManager::sInstance->Update();

	StackManager::sInstance->Update(Timing::sInstance.GetDeltaTime());

	if (StackManager::sInstance->IsEmpty())
	{
		PlayerDataManager::sInstance->Save();
		WindowManager::sInstance->close();
	}

	RenderManager::sInstance->Render();
}

void Client::HandleEvent(sf::Event& p_event)
{
	switch (p_event.type)
	{
	case sf::Event::KeyPressed:
		InputManager::sInstance->HandleInput(EIA_Pressed, p_event.key.code);
		break;
	case sf::Event::KeyReleased:
		InputManager::sInstance->HandleInput(EIA_Released, p_event.key.code);
		break;
	default:
		break;
	}

	StackManager::sInstance->HandleEvent(p_event);
	if (p_event.type == sf::Event::Closed)
	{
		PlayerDataManager::sInstance->Save();
		WindowManager::sInstance->close();
	}
}

bool Client::PollEvent(sf::Event& p_event)
{
	return WindowManager::sInstance->pollEvent(p_event);
}
