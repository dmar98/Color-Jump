#include "RoboCatClientPCH.hpp"

MenuState::MenuState(StateStack& stack): State(stack)
{
	const auto texture = TextureManager::sInstance->GetTexture("background");

	m_background_sprite.setTexture(*texture);

	std::shared_ptr<GUI::Button> join_button;
	Utility::CreateButton(join_button, 100, 350, "Join", [this]
		{
			RequestStackPop();
			RequestStackPush(StateID::kJoinSettings);
		});

	std::shared_ptr<GUI::Button> settings_button;
	Utility::CreateButton(settings_button, 100, 400, "Settings", [this]
		{
			RequestStackPush(StateID::kSettings);
		});

	std::shared_ptr<GUI::Button> exit_button;
	Utility::CreateButton(exit_button, 100, 450, "Exit", [this]
		{
			RequestStackPop();
		});
	
	m_gui_container.Pack(join_button);
	m_gui_container.Pack(settings_button);
	m_gui_container.Pack(exit_button);
}

void MenuState::Draw()
{
	WindowManager::sInstance->draw(m_background_sprite);
	WindowManager::sInstance->draw(m_gui_container);
}

bool MenuState::Update(float dt)
{
	return true;
}

bool MenuState::HandleEvent(const sf::Event& event)
{
	m_gui_container.HandleEvent(event);
	return false;
}
