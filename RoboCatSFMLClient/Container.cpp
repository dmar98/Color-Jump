#include "ColorJumpClientPCH.hpp"

namespace GUI
{
	Container::Container()
		: m_selected_child(-1)
	{
	}

	void Container::Pack(const Component::Ptr& component)
	{
		m_children.emplace_back(component);
		if (!HasSelection() && component->IsSelectable() && component->CanBeDrawn())
		{
			Select(m_children.size() - 1);
		}
	}

	bool Container::IsSelectable() const
	{
		return false;
	}

	void Container::HandleEvent(const sf::Event& event)
	{
		if (HasSelection() && m_children[m_selected_child]->IsActive())
		{
			m_children[m_selected_child]->HandleEvent(event);
		}
		else if (event.type == sf::Event::KeyReleased)
		{
			if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up)
			{
				SelectPrevious();
			}
			else if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down)
			{
				SelectNext();
			}
			else if (event.key.code == sf::Keyboard::Return || event.key.code ==
				sf::Keyboard::Space)
			{
				if (HasSelection())
				{
					m_children[m_selected_child]->Activate();
				}
			}
		}
	}

	void Container::Pull(const Component::Ptr& component)
	{
		auto& components = m_children;
		const auto remove = std::remove(components.begin(), components.end(), component);
		components.erase(remove, components.end());
	}

	void Container::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		for (const Component::Ptr& child : m_children)
		{
			if (child->CanBeDrawn())
				target.draw(*child, states);
		}
	}

	bool Container::HasSelection() const
	{
		return m_selected_child >= 0;
	}

	void Container::Select(const std::size_t index)
	{
		if (index < m_children.size() && m_children[index]->IsSelectable())
		{
			if (HasSelection())
			{
				m_children[m_selected_child]->Deselect();
			}
			m_children[index]->Select();
			m_selected_child = index;
		}
	}

	void Container::SelectNext()
	{
		if (!HasSelection())
		{
			return;
		}
		//Search for the next component that is selectable and wrap around if necessary
		int next = m_selected_child;
		do
		{
			next = (next + 1) % m_children.size();
		}
		while (!m_children[next]->IsSelectable() || !m_children[next]->CanBeDrawn());

		Select(next);
	}

	void Container::SelectPrevious()
	{
		if (!HasSelection())
		{
			return;
		}
		//Search for the next component that is selectable and wrap around if necessary
		int prev = m_selected_child;
		do
		{
			prev = (prev + m_children.size() - 1) % m_children.size();
		}
		while (!m_children[prev]->IsSelectable() || !m_children[prev]->CanBeDrawn());

		Select(prev);
	}
}
