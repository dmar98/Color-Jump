#include "RoboCatClientPCH.hpp"


namespace GUI
{
	/*
	 *	Dylan Goncalves Martins (D00242562)
	 *	Button color now can changed
	 */
	void Button::ChangeTextColor(const ButtonType button)
	{
		switch (button)
		{
		case ButtonType::Normal:
			m_text.setFillColor(sf::Color::White);
			break;
		case ButtonType::Selected:
			m_text.setFillColor(sf::Color(231, 150, 53));
			break;
		case ButtonType::Pressed:
			m_text.setFillColor(sf::Color(122, 193, 67));
			break;
		case ButtonType::ButtonCount: break;
		}
	}

	Button::Button()
		: m_sprite(*TextureManager::sInstance->GetTexture("button"))
		  , m_text("", *FontManager::sInstance->GetFont("carlito"), 16)
		  , m_is_toggle(false)
	{
		ChangeTexture(ButtonType::Normal);
		const sf::FloatRect bounds = m_sprite.getLocalBounds();
		m_text.setPosition(bounds.width / 2, bounds.height / 2);
	}

	void Button::SetCallback(Callback callback)
	{
		m_callback = std::move(callback);
	}

	void Button::SetText(const std::string& text)
	{
		m_text.setString(text);
	}

	void Button::SetToggle(const bool flag)
	{
		m_is_toggle = flag;
	}

	bool Button::IsSelectable() const
	{
		return true;
	}

	void Button::Select()
	{
		Component::Select();
		ChangeTexture(ButtonType::Selected);
	}

	void Button::Deselect()
	{
		Component::Deselect();
		ChangeTexture(ButtonType::Normal);
	}

	void Button::Activate()
	{
		Component::Activate();
		//If toggle then show button is pressed or toggled
		if (m_is_toggle)
		{
			ChangeTexture(ButtonType::Pressed);
		}
		if (m_callback)
		{
			m_callback();
		}
		if (!m_is_toggle)
		{
			Deactivate();
		}
	}

	void Button::Deactivate()
	{
		Component::Deactivate();
		if (m_is_toggle)
		{
			if (IsSelected())
			{
				ChangeTexture(ButtonType::Selected);
			}
			else
			{
				ChangeTexture(ButtonType::Normal);
			}
		}
	}

	void Button::HandleEvent(const sf::Event& event)
	{
	}

	void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		target.draw(m_sprite, states);
		target.draw(m_text, states);
	}

	void Button::ChangeTexture(ButtonType buttonType)
	{
		const sf::IntRect textureRect(0, 50 * static_cast<int>(buttonType), 200, 50);
		m_sprite.setTextureRect(textureRect);
		ChangeTextColor(buttonType);
	}
}
