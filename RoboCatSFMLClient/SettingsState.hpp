#pragma once
class SettingsState final : public State
{
public:
	auto HandleBackCallback() const;
	explicit SettingsState();

	void Draw() override;
	bool Update(float dt) override;
	bool HandleEvent(const sf::Event& event) override;

private:
	void UpdateLabels() const;
	void AddButtonLabel(std::size_t index, size_t x, size_t y, const std::string& text);

private:
	sf::Sprite m_background_sprite;
	GUI::Container m_gui_container;
	std::array<GUI::Button::Ptr, static_cast<int>(PlayerAction::kActionCount)> m_binding_buttons;
	std::array<GUI::Label::Ptr, static_cast<int>(PlayerAction::kActionCount)> m_binding_labels;
};
