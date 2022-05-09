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

	void SetTeamIdentifier(sf::Int8 identifier) override;
	void SetName(const std::string& name) override;

	sf::IntRect GetSize() const;
	sf::FloatRect GetBoundingRect() const override;
	void Jump() override;
	void SetGrounded(Platform* platform) override;
	void SetGrounded() override;
	void MoveOutOfCollision(const sf::FloatRect& rect);
	RayGround* GetRay() const;

private:
	void UpdateRay() const;
	void CreateRay();

	SpriteComponentPtr mSpriteComponent;
	TextComponentPtr m_name_text;
	TextComponentPtr m_team_id_text;

	bool m_show_jump_animation{};
	/*Animation m_jump_smoke_animation;*/
};

