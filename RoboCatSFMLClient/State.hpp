#pragma once
class StateStack;

class State
{
public:
	using Ptr = std::unique_ptr<State>;

	explicit State(StateStack& stack);
	virtual ~State();
	virtual bool Update(float dt) = 0;
	virtual bool HandleEvent(const sf::Event& event) = 0;
	virtual void OnStackPopped();
	void RequestStackPush(StateID state_id) const;
	virtual void Draw() = 0;

protected:
	void RequestStackPop() const;
	void RequestStackClear() const;

private:
	StateStack* m_stack;
};
