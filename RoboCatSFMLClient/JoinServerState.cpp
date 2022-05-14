#include "ColorJumpClientPCH.hpp"

auto JoinServerState::HandleConnectButton() const
{
	return [this]
	{
		RequestStackPop();
		RequestStackPush(StateID::kLobby);
	};
}

auto JoinServerState::HandleBackButton() const
{
	return [this]
	{
		RequestStackPop();
		RequestStackPush(StateID::kMenu);
	};
}

//Written by Paul Bichler (D00242563)
JoinServerState::JoinServerState() : State()
                                     , m_player_input_ip(PlayerDataManager::sInstance->GetIP())
{
	m_background_sprite.setTexture(*TextureManager::sInstance->GetTexture("background"));

	constexpr int button_x = 80;
	constexpr int button_y = 300;
	constexpr int label_x = 310;
	constexpr int label_height = 15;
	constexpr int space = 55;
	constexpr int button_height = 50;


	Utility::CreateButton(m_change_ip_button, button_x, button_y, "IP Address", true);
	m_gui_container.Pack(m_change_ip_button);


	Utility::CreateLabel(m_current_ip_label, label_x, button_y + label_height, m_player_input_ip,
	                     20, false);
	m_gui_container.Pack(m_current_ip_label);

	auto button = std::make_shared<GUI::Button>();


	Utility::CreateButton(button, button_x, button_y + button_height + space, "Connect",
	                      HandleConnectButton());
	m_gui_container.Pack(button);

	Utility::CreateButton(button, button_x, button_y + button_height + 2 * space, "Back",
	                      HandleBackButton());
	m_gui_container.Pack(button);
}

bool JoinServerState::Update(float dt)
{
	return true;
}

bool JoinServerState::HandleEvent(const sf::Event& event)
{
	if (m_change_ip_button->IsActive())
	{
		//Ip Input
		if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Return)
		{
			//Make Ip Address persistent by saving it once input in finished
			m_change_ip_button->Deactivate();
			PlayerDataManager::sInstance->SetIp(m_player_input_ip);
			PlayerDataManager::sInstance->Save();
		}
		else if (event.type == sf::Event::TextEntered)
		{
			if (event.text.unicode == '\b')
			{
				//Handle backspace
				if (!m_player_input_ip.empty())
					m_player_input_ip.erase(m_player_input_ip.size() - 1, 1);
			}
			else if (event.text.unicode != '\n' && event.text.unicode != '\r')
			{
				//Handle player text input (new lines and carriage returns are not allowed!)
				m_player_input_ip += event.text.unicode;
				m_player_input_ip = m_player_input_ip.substr(0, 25);
			}

			m_current_ip_label->SetText(m_player_input_ip);
		}
	}
	else
	{
		m_gui_container.HandleEvent(event);
	}

	return false;
}

void JoinServerState::Draw()
{
	WindowManager::sInstance->draw(m_background_sprite);
	WindowManager::sInstance->draw(m_gui_container);
}
