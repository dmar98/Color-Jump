#pragma once
class InputManager
{
public:
	static void StaticInit();
	static unique_ptr<InputManager> sInstance;
	


	bool GetPassFocus() const {return m_pass_focus;}
	void SetPassFocus(bool pass_focus);

private:
	bool m_pass_focus;
	InputManager();
};
