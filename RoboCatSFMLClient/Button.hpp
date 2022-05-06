#pragma once

namespace GUI
{
	class Button final : public Component
	{
	public:
		typedef std::shared_ptr<Button> Ptr;
		typedef std::function<void()> Callback;

		void ChangeTextColor(ButtonType button);
		explicit Button();
		void SetCallback(Callback callback);
		void SetText(const std::string& text);
		void SetToggle(bool flag);

		bool IsSelectable() const override;
		void Select() override;
		void Deselect() override;
		void Activate() override;
		void Deactivate() override;
		void HandleEvent(const sf::Event& event) override;

	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
		void ChangeTexture(ButtonType buttonType);

		Callback m_callback;
		sf::Sprite m_sprite;
		sf::Text m_text;
		bool m_is_toggle;
	};
}

