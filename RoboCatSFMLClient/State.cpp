#include "RoboCatClientPCH.hpp"

State::State() = default;

State::~State() = default;


void State::OnStackPopped()
{
	//Does nothing. Can be overridden in child classes to do something if the state is popped.
	//(This is needed because certain operations can't be done in the destructor)
}

void State::RequestStackPush(const StateID state_id)
{
	StackManager::sInstance->PushState(state_id);
}

void State::RequestStackPop()
{
	StackManager::sInstance->PopState();
}

void State::RequestStackClear()
{
	StackManager::sInstance->ClearStates();
}
