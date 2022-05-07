#pragma once

class Utility
{
	static void CreateButton(std::shared_ptr<GUI::Button>& play_button, int x,
	                         int y, const std::string& label, const GUI::Button::Callback& callback,
	                         bool toggle, const std::function<bool()>& predicate);
public:
	static int RandomInt(int exclusive_max);
	static void CentreOrigin(sf::Sprite& sprite);
	static void CentreOrigin(sf::Text& text);
	static std::string ToString(sf::Keyboard::Key key);
	static double ToRadians(int degrees);
	static sf::Vector2f UnitVector(sf::Vector2f vector);
	static float Length(sf::Vector2f vector);
	static float ToDegrees(float angle);
	static void Debug(const std::string& message);
	static void CreateButton(std::shared_ptr<GUI::Button>& button, int x, int y,
	                         const std::string& label, const GUI::Button::Callback& callback,
	                         const std::function<bool()>& predicate = nullptr);
	static void CreateButton(std::shared_ptr<GUI::Button>& button, int x, int y,
	                         const std::string& label, bool toggle);

	static void CreateLabel(std::shared_ptr<GUI::Label>& label, int x, int y,
	                        const std::string& label_text, int text_size);
	static void CreateLabel(std::shared_ptr<GUI::Label>& label, int x, int y, const string& label_text, int text_size,
	                bool centered);
};
