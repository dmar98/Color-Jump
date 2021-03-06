#pragma once
class CommandQueue
{
public:
	void Push(const Command& command);
	Command Pop();
	bool IsEmpty() const;

private:
	std::queue<Command> m_queue;
};

