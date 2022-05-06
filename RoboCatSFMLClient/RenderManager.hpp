//I take care of rendering things!

#include "Client.hpp"

class RenderManager
{
public:
	static void StaticInit();
	static std::unique_ptr<RenderManager> sInstance;

	static void Render();
	void RenderComponents() const;

	//vert inefficient method of tracking scene graph...
	void AddComponent(SpriteComponent* inComponent);
	void RemoveComponent(SpriteComponent* inComponent);
	int GetComponentIndex(SpriteComponent* inComponent) const;
	static void Render(const StateStack* stack);

private:
	RenderManager();

	//this can't be only place that holds on to component- it has to live inside a GameObject in the world
	vector<SpriteComponent*> mComponents;

	sf::View view;
};
