#include "RoboCatClientPCH.hpp"

std::unique_ptr<StackManager> StackManager::sInstance;

bool StackManager::StaticInit()
{
	sInstance.reset(new StackManager());
	return true;
}

void StackManager::PushState(const StateID title)
{
	m_stack.PushState(title);
}

void StackManager::Update(const float delta_time)
{
	m_stack.Update(delta_time);
}

bool StackManager::IsEmpty() const
{
	return m_stack.IsEmpty();
}

void StackManager::HandleEvent(const sf::Event& p_event)
{
	m_stack.HandleEvent(p_event);
}

void StackManager::Render() const
{
	m_stack.Render();
}

void StackManager::PopState()
{
	m_stack.PopState();
}

void StackManager::ClearStates()
{
	m_stack.ClearStates();
}

State* StackManager::GetCurrentState() const
{
	return m_stack.Current();
}

StackManager::StackManager()
{
	m_stack.RegisterState<TitleState>(StateID::kTitle);
	m_stack.RegisterState<MenuState>(StateID::kMenu);
	m_stack.RegisterState<JoinServerState>(StateID::kJoinSettings);
	m_stack.RegisterState<SettingsState>(StateID::kSettings);
	m_stack.RegisterState<LobbyState>(StateID::kLobby);

	m_stack.PushState(StateID::kTitle);
}


