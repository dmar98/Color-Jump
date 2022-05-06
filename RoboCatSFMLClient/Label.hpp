#pragma once
namespace GUI
{
	class Label final : public Component
	{
	public:
		using Ptr = std::shared_ptr<Label>;
		explicit Label(const std::string& text, int font_size = 16);
		bool IsSelectable() const override;
		void SetText(const std::string& text);
		void HandleEvent(const sf::Event& event) override;
		sf::Text& GetText();

	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
		sf::Text m_text;
	};
}
