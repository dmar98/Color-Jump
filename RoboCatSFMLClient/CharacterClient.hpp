#pragma once
#include "TextComponent.hpp"

class CharacterClient : public Character
{
public:
	CharacterClient(EColorType type, const sf::IntRect& texture_rect);

	void InitRay();

	void Update() override;
	void HandleDying() override;

	void Read(InputMemoryBitStream& inInputStream) override;

	void SetTransparent() const;
	void SetOpaque() const;

	void SetTeamIdentifier(const int identifier) override;
	void SetName(const std::string& name) override;

	sf::IntRect GetSize() const;
	virtual sf::FloatRect GetBoundingRect() const;
	void Jump() override;
	void SetGrounded(Platform* platform) override;
	void SetGrounded() override;
	void MoveOutOfCollision(const sf::Time dt, const sf::FloatRect& rect);

private:
	void UpdateRay() const;
	void CreateRay();

	/*sf::Text m_name_text;
	sf::Text m_team_id_text;*/
	SpriteComponentPtr mSpriteComponent;
	TextComponentPtr m_name_text;
	TextComponentPtr m_team_id_text;

	bool m_show_jump_animation{};
	/*Animation m_jump_smoke_animation;*/
};

