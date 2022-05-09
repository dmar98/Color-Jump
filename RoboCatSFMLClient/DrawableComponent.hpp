#pragma once

class DrawableComponent : public sf::Drawable, public sf::Transformable
{
public:
	explicit DrawableComponent(GameObject* game_object);
	~DrawableComponent() override;

	void UpdatePosition();
	void SetDrawPredicate(const std::function<bool()>& predicate);

	virtual void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const = 0;

protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	GameObject* m_game_object;
	std::function<bool()> m_draw_predicate;
};

