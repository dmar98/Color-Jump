#include "ColorJumpClientPCH.hpp"

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
	World::StaticInit(new WorldClient());

	s_instance.reset(client);

	return true;
}

Client::Client() = default;

void Client::DoFrame()
{
	StackManager::sInstance->Update(Timing::sInstance.GetDeltaTime());

	if (StackManager::sInstance->IsEmpty())
	{
		PlayerDataManager::sInstance->Save();
		Quit();
		WindowManager::sInstance->close();
	}

	RenderManager::sInstance->Render();
}

void Client::HandleEvent(sf::Event& p_event)
{
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
