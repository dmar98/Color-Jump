#pragma once

class TitleState final : public State
{
public:
	explicit TitleState(StateStack& stack);
	bool Update(float dt) override;
	bool HandleEvent(const sf::Event& event) override;
	void Draw() override;

private:
	sf::Sprite m_background_sprite;
	sf::Text m_text;

	bool m_show_text;
	float m_text_effect_time;
	SpriteComponentPtr mSpriteComponent;
};
