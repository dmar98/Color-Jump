#include "ColorJumpClientPCH.hpp"

MenuState::MenuState(): State()
{
	m_background_sprite.setTexture(*TextureManager::sInstance->GetTexture("background"));

	std::shared_ptr<GUI::Button> button;
	constexpr int x = 100;
	constexpr int y = 350;
	constexpr int space = 55;

	Utility::CreateButton(button, x, y, "Join", [this]
	{
		RequestStackPop();
		RequestStackPush(StateID::kJoinSettings);
	});
	m_container.Pack(button);

	Utility::CreateButton(button, x, y + space, "Settings", [this]
	{
		RequestStackPush(StateID::kSettings);
	});
	m_container.Pack(button);

	Utility::CreateButton(button, x, y + space * 2, "Exit", [this]
	{
		RequestStackPop();
	});
	m_container.Pack(button);

	// Play menu theme
	AudioManager::sInstance->PlayMusic("MenuTheme");
}

void MenuState::Draw()
{
	WindowManager::sInstance->draw(m_background_sprite);
	WindowManager::sInstance->draw(m_container);
}

bool MenuState::Update(float dt)
{
	return true;
}

bool MenuState::HandleEvent(const sf::Event& event)
{
	m_container.HandleEvent(event);
	return false;
}
