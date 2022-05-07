#include "RoboCatClientPCH.hpp"


auto SettingsState::HandleBackCallback() const
{
	return [this]
	{
		RequestStackPop();
	};
}

SettingsState::SettingsState() : State()
{
	m_background_sprite.setTexture(*TextureManager::sInstance->GetTexture("background"));

	std::shared_ptr<GUI::Label> label;
	Utility::CreateLabel(label, 80, 230, "Player", 25, false);
	m_gui_container.Pack(label);

	AddButtonLabel(static_cast<int>(PlayerAction::kMoveLeft), 0, 0, "Move Left");
	AddButtonLabel(static_cast<int>(PlayerAction::kMoveRight), 0, 1, "Move Right");
	AddButtonLabel(static_cast<int>(PlayerAction::kMoveUp), 0, 2, "Move Up");

	UpdateLabels();

	std::shared_ptr<GUI::Button> button;
	Utility::CreateButton(button, 80, 520, "Back", HandleBackCallback());
	m_gui_container.Pack(button);
}

void SettingsState::Draw()
{
	WindowManager::sInstance->draw(m_background_sprite);
	WindowManager::sInstance->draw(m_gui_container);
}

bool SettingsState::Update(float dt)
{
	return true;
}

bool SettingsState::HandleEvent(const sf::Event& event)
{
	bool is_key_binding = false;

	// Iterate through all key binding buttons to see if they are being pressed, waiting for the user to enter a key
	for (std::size_t i = 0; i < m_binding_buttons.size(); ++i)
	{
		if (m_binding_buttons[i]->IsActive())
		{
			is_key_binding = true;
			if (event.type == sf::Event::KeyReleased)
			{
				KeyBinding::sInstance->AssignKey(static_cast<PlayerAction>(i), event.key.code);
				m_binding_buttons[i]->Deactivate();
				PlayerDataManager::sInstance->Save();
			}
			break;
		}
	}

	if (is_key_binding)
	{
		UpdateLabels();
	}
	else
	{
		m_gui_container.HandleEvent(event);
	}
	return false;
}

void SettingsState::UpdateLabels() const
{
	for (std::size_t i = 0; i < static_cast<int>(PlayerAction::kActionCount); ++i)
	{
		const auto action = static_cast<PlayerAction>(i);
		m_binding_labels[i]->SetText(
			Utility::ToString(KeyBinding::sInstance->GetAssignedKey(action)));
	}
}

void SettingsState::AddButtonLabel(std::size_t index, const size_t x, const size_t y,
                                   const std::string& text)
{
	index += static_cast<int>(PlayerAction::kActionCount) * x;

	m_binding_buttons[index] = std::make_shared<GUI::Button>();
	const auto x_pos = static_cast<float>(x);
	const auto y_pos = static_cast<float>(y);

	Utility::CreateButton(m_binding_buttons[index], 400 * x_pos + 80, 50 * y_pos + 300, text, true);
	Utility::CreateLabel(m_binding_labels[index], 400 * x_pos + 300, 50 * y_pos + 315, "", 20,
	                     false);

	m_gui_container.Pack(m_binding_buttons[index]);
	m_gui_container.Pack(m_binding_labels[index]);
}
