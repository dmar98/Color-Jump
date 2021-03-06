#pragma once
class RenderManager
{
public:
	static void StaticInit();
	static std::unique_ptr<RenderManager> sInstance;

	static void Render();
	void RenderComponents(const sf::View& camera) const;

	//vert inefficient method of tracking scene graph...
	void AddComponent(DrawableComponent* inComponent);
	void RemoveComponent(DrawableComponent* inComponent);
	int GetComponentIndex(DrawableComponent* inComponent) const;
	void SetRay(RayGround* ray);

private:
	RenderManager();

	//this can't be only place that holds on to component- it has to live inside a GameObject in the world
	vector<DrawableComponent*> mComponents;

	sf::View view;
	RayGround* mRay;
};
