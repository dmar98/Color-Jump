#include "RoboCatClientPCH.hpp"

//Written by Paul Bichler (D00242563)
JoinServerState::JoinServerState(StateStack& stack)
	: State(stack),
	m_player_input_ip("127.0.0.1:50005")
{
	const TexturePtr texture = TextureManager::sInstance->GetTexture("mouse");
	m_background_sprite.setTexture(*texture);

	//Ip Input
	m_change_ip_button = std::make_shared<GUI::Button>();
	m_change_ip_button->SetText("IP Address");
	m_change_ip_button->SetToggle(true);
	m_change_ip_button->setPosition(80.f, 300.f);

	m_current_ip_label = std::make_shared<GUI::Label>(m_player_input_ip, 20);
	m_current_ip_label->setPosition(310.f, 315.f);

	m_gui_container.Pack(m_change_ip_button);
	m_gui_container.Pack(m_current_ip_label);

	//Connect Button
	const auto connect_button = std::make_shared<GUI::Button>();
	connect_button->setPosition(80.f, 400.f);
	connect_button->SetText("Connect");
	connect_button->SetCallback([this]
	{
		RequestStackPop(); //Pop Menu State
		RequestStackPush(StateID::kLobbyClient);
	});

	m_gui_container.Pack(connect_button);

	//Back Button
	const auto back_button = std::make_shared<GUI::Button>();
	back_button->setPosition(80.f, 450.f);
	back_button->SetText("Back");
	back_button->SetCallback([this]
	{
		RequestStackPop();
		RequestStackPush(StateID::kMenu);
	});

	m_gui_container.Pack(back_button);
}

//Written by Paul Bichler (D00242563)
bool JoinServerState::Update(float dt)
{
	return true;
}

//Written by Paul Bichler (D00242563)
bool JoinServerState::HandleEvent(const sf::Event& event)
{
	if (m_change_ip_button->IsActive())
	{
		//Ip Input
		if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Return)
		{
			//Make Ip Address persistent by saving it once input in finished
			m_change_ip_button->Deactivate();
			// GetContext().m_player_data_manager->GetData().m_ip_address = m_player_input_ip;
			// GetContext().m_player_data_manager->Save();
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
}
