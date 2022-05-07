#pragma once
class StackManager
{
public:
	static bool StaticInit();
	void PushState(StateID title);
	void Update(float delta_time);
	bool IsEmpty() const;
	void HandleEvent(const sf::Event& p_event);
	void Render() const;
	void PopState();
	void ClearStates();
	static std::unique_ptr<StackManager> sInstance;
private:
	StackManager();

	StateStack m_stack;
};

