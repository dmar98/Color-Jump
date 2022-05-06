#include "RoboCatClientPCH.hpp"


TitleState::TitleState(StateStack& stack)
	: State(stack)
	  , m_show_text(true)
	  , m_text_effect_time(0)
{
	const auto texture_ptr = TextureManager::sInstance->GetTexture("background");
	m_background_sprite.setTexture(*texture_ptr);

	m_text.setFont(*FontManager::sInstance->GetFont("carlito"));
	m_text.setString("Press any key to continue");
	const sf::FloatRect bounds = m_text.getLocalBounds();
	m_text.setOrigin(std::floor(bounds.left + bounds.width / 2.f),
	                 std::floor(bounds.top + bounds.height / 2.f));
	m_text.setPosition(WindowManager::sInstance->getView().getSize() / 2.f);
}

bool TitleState::Update(const float dt)
{
	m_text_effect_time += dt;

	if (m_text_effect_time >= .5f)
	{
		m_show_text = !m_show_text;
		m_text_effect_time = 0;
	}
	return true;
}

bool TitleState::HandleEvent(const sf::Event& event)
{
	if (event.type == sf::Event::KeyReleased)
	{
		RequestStackPop();
		RequestStackPush(StateID::kMenu);
	}
	return false;
}

void TitleState::Draw()
{
	WindowManager::sInstance->draw(m_background_sprite);
	if (m_show_text)
	{
		WindowManager::sInstance->draw(m_text);
	}
}
