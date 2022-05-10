#include "RoboCatClientPCH.hpp"

std::unique_ptr<RenderManager> RenderManager::sInstance;

RenderManager::RenderManager()
{
	view.reset(sf::FloatRect(0, 0, 1280, 720));
	WindowManager::sInstance->setView(view);
}


void RenderManager::StaticInit()
{
	sInstance.reset(new RenderManager());
}


void RenderManager::AddComponent(DrawableComponent* inComponent)
{
	mComponents.emplace_back(inComponent);
}

void RenderManager::RemoveComponent(DrawableComponent* inComponent)
{
	const int index = GetComponentIndex(inComponent);

	if (index != -1)
	{
		const int lastIndex = mComponents.size() - 1;
		if (index != lastIndex)
		{
			mComponents[index] = mComponents[lastIndex];
		}
		mComponents.pop_back();
	}
}

int RenderManager::GetComponentIndex(DrawableComponent* inComponent) const
{
	for (int i = 0, c = mComponents.size(); i < c; ++i)
	{
		if (mComponents[i] == inComponent)
		{
			return i;
		}
	}

	return -1;
}

void RenderManager::SetRay(RayGround* ray)
{
	mRay = ray;
}

//this part that renders the world is really a camera-
//in a more detailed engine, we'd have a list of cameras, and then render manager would
//render the cameras in order
void RenderManager::RenderComponents(const sf::View& camera) const
{
	WindowManager::sInstance->setView(camera);

	//Get the logical viewport so we can pass this to the SpriteComponents when it's draw time
	for (DrawableComponent* c : mComponents)
	{
		c->UpdatePosition();
		WindowManager::sInstance->draw(*c);
	}

	if (mRay)
	{
		sf::RectangleShape shape;
		shape.setPosition(sf::Vector2f(mRay->GetBoundingRect().left, mRay->GetBoundingRect().top));
		shape.setSize(sf::Vector2f(mRay->GetBoundingRect().width, mRay->GetBoundingRect().height));
		shape.setFillColor(sf::Color::Transparent);
		shape.setOutlineColor(sf::Color::Green);
		shape.setOutlineThickness(1.f);
		WindowManager::sInstance->draw(shape);
	}

}

void RenderManager::Render()
{
	WindowManager::sInstance->clear(sf::Color(0, 37, 97, 255));
	StackManager::sInstance->Render();
	WindowManager::sInstance->display();

}
