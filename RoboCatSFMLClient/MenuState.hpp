#pragma once

class MenuState final : public State
{
public:
	explicit MenuState();
	void Draw() override;
	bool Update(float dt) override;
	bool HandleEvent(const sf::Event& event) override;

private:
	sf::Sprite m_background_sprite;
	GUI::Container m_container;
};
