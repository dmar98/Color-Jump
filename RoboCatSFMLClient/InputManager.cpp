#include "ColorJumpClientPCH.hpp"

unique_ptr<InputManager> InputManager::sInstance;

void InputManager::StaticInit()
{
	sInstance.reset(new InputManager());
}


void InputManager::SetPassFocus(const bool pass_focus)
{
	m_pass_focus = pass_focus;
}

InputManager::InputManager()
	: m_pass_focus(true)
{
}