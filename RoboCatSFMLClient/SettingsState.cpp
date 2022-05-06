#include "RoboCatClientPCH.hpp"


SettingsState::SettingsState(StateStack& stack) : State(stack)
{
}

void SettingsState::Draw()
{
}

bool SettingsState::Update(float dt)
{
	return true;
}

bool SettingsState::HandleEvent(const sf::Event& event)
{
	return false;
}

void SettingsState::UpdateLabels()
{
}

void SettingsState::AddButtonLabel(std::size_t index, const size_t x, const size_t y, const std::string& text)
{
}
