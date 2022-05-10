#pragma once

struct Command
{
	Command();
	std::function<void(GameObject&)> action;
	unsigned int category;
};

template<typename Object, typename Function>
std::function<void(GameObject&)> DerivedAction(Function fn)
{
	return [=](GameObject& gameobject)
	{
		//Check if cast is safe
		assert(dynamic_cast<Object*>(&gameobject) != nullptr);

		//Downcast gameobject and invoke the function
		fn(static_cast<Object&>(gameobject));
	};
}



