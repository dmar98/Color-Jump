#pragma once
class SettingsState final : public State
{
public:
	explicit SettingsState(StateStack& stack);

	void Draw();
	bool Update(float dt) override;
	bool HandleEvent(const sf::Event& event) override;

private:
	void UpdateLabels();
	void AddButtonLabel(std::size_t index, size_t x, size_t y, const std::string& text);

private:
	sf::Sprite m_background_sprite;
	GUI::Container m_gui_container;
	// std::array<GUI::Button::Ptr, 2 * static_cast<int>(PlayerAction::kActionCount)> m_binding_buttons;
	// std::array<GUI::Label::Ptr, 2 * static_cast<int>(PlayerAction::kActionCount)> m_binding_labels;
};
