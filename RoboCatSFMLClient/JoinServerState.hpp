#pragma once
class JoinServerState final : public State
{
public:
	auto HandleConnectButton() const;
	auto HandleBackButton() const;
	explicit JoinServerState();
	bool Update(float dt) override;
	bool HandleEvent(const sf::Event& event) override;
	void Draw() override;

private:
	sf::Sprite m_background_sprite;
	GUI::Container m_gui_container{};

	GUI::Button::Ptr m_change_ip_button{};
	GUI::Label::Ptr m_current_ip_label{};
	std::string m_player_input_ip;

	SpriteComponentPtr mSpriteComponent;
};

