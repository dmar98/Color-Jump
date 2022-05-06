#include "RoboCatClientPCH.hpp"

bool Client::StaticInit()
{
	// Create the Client pointer first because it initializes SDL
	auto* client = new Client();
	InputManager::StaticInit();

	WindowManager::StaticInit();
	FontManager::StaticInit();
	TextureManager::StaticInit();
	RenderManager::StaticInit();
	

	HUD::StaticInit();

	s_instance.reset(client);

	return true;
}

Client::Client() : m_stack(new StateStack())
{
	RegisterStates();
	m_stack->PushState(StateID::kTitle);
}

void Client::RegisterStates() const
{
	m_stack->RegisterState<TitleState>(StateID::kTitle);
	m_stack->RegisterState<MenuState>(StateID::kMenu);
}

void Client::DoFrame()
{
	InputManager::sInstance->Update();

	// Engine::DoFrame();

	// NetworkManagerClient::sInstance->ProcessIncomingPackets();
	m_stack->Update(Timing::sInstance.GetDeltaTime());
	RenderManager::sInstance->Render(m_stack);

	// NetworkManagerClient::sInstance->SendOutgoingPackets();
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

	m_stack->HandleEvent(p_event);
}

bool Client::PollEvent(sf::Event& p_event)
{
	return WindowManager::sInstance->pollEvent(p_event);
}


