#include "RoboCatClientPCH.hpp"

TutorialState::TutorialState() : State()
{
	const int middle_x = WindowManager::sInstance->getSize().x / 2;

	//Create title text
	std::shared_ptr<GUI::Label> label;
	Utility::CreateLabel( label, 550, 50, "Welcome to Color Jump!", 60, false);
	m_gui_container.Pack(label);

	//Create tutorial text
	Utility::CreateLabel(label, 500, 200,
		"In Color Jump, you play in a team of 2, one Red and one Blue character. Characters can\nonly collide with platforms of their color or white platforms.\n\n\n\n\n\n\n\nSome platforms change their color to the color of the character that collides with them.\nThe goal of Color Jump is for your team to complete the obstacle course and reach the end\nplatform before any other team. The level features some tricky puzzles that require you and\nyour team mate to carefully think about your moves. Made a wrong move? Jumping out of\nthe play field resets the puzzle for both players.\n\n\n\n\n\n\n\n\nDon't worry, there's a checkpoint after each puzzle that is activated when both players stand\non it (it turns green if it's activated).\n\nHappy Jumping!"
		, 20, false);
	m_gui_container.Pack(label);

	//Create Back Button
	std::shared_ptr<GUI::Button> back_button;
	Utility::CreateButton(back_button, middle_x - 100, 850, "OK", [this]
		{
			RequestStackPop();
		});
	m_gui_container.Pack(back_button);

	//Add tutorial images
	m_tutorial1.setTexture(*TextureManager::sInstance->GetTexture("Tutorial 1"));
	m_tutorial1.setScale(.37f, .37f);
	m_tutorial1.setPosition(100, 50);

	m_tutorial2.setTexture(*TextureManager::sInstance->GetTexture("Tutorial 2"));
	m_tutorial2.setScale(.37f, .37f);
	m_tutorial2.setPosition(100, 350);

	m_tutorial3.setTexture(*TextureManager::sInstance->GetTexture("Tutorial 3"));
	m_tutorial3.setScale(.37f, .37f);
	m_tutorial3.setPosition(100, 650);
}

void TutorialState::Draw()
{
	WindowManager::sInstance->clear(sf::Color(0, 37, 80));
	WindowManager::sInstance->draw(m_gui_container);
	WindowManager::sInstance->draw(m_tutorial1);
	WindowManager::sInstance->draw(m_tutorial2);
	WindowManager::sInstance->draw(m_tutorial3);
}

bool TutorialState::Update(float dt)
{
	return true;
}

bool TutorialState::HandleEvent(const sf::Event& event)
{
	m_gui_container.HandleEvent(event);
	return false;
}
