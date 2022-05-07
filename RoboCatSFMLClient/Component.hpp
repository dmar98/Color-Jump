#pragma once

namespace GUI
{
	class Component : public sf::Drawable, public sf::Transformable, private sf::NonCopyable
	{
	public:
		using Ptr = std::shared_ptr<Component>;

	public:
		Component();
		~Component() override = default;

		virtual bool IsSelectable() const = 0;
		bool IsSelected() const;
		virtual void Select();
		virtual void Deselect();
		virtual bool IsActive() const;
		virtual void Activate();
		virtual void Deactivate();

		virtual void HandleEvent(const sf::Event& event) = 0;
		void SetDrawPredicate(const std::function<bool()>& predicate);
		bool CanBeDrawn() const;

	private:
		bool m_is_selected;
		bool m_is_active;
		std::function<bool()> m_draw_predicate;
	};
}
