#include "RoboCatClientPCH.hpp"

State::State(StateStack& stack) : m_stack(&stack)
{
}

State::~State() = default;


void State::OnStackPopped()
{
	//Does nothing. Can be overridden in child classes to do something if the state is popped.
	//(This is needed because certain operations can't be done in the destructor)
}

void State::RequestStackPush(const StateID state_id) const
{
	m_stack->PushState(state_id);
}

void State::RequestStackPop() const
{
	m_stack->PopState();
}

void State::RequestStackClear() const
{
	m_stack->ClearStates();
}
