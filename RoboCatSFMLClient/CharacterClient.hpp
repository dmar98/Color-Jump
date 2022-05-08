#pragma once
class CharacterClient : public Character
{
public:
	CharacterClient(EColorType type, const sf::IntRect& texture_rect);

	void InitRay();

	void Update() override;
	void HandleDying() override;

	void Read(InputMemoryBitStream& inInputStream) override;

	void SetColor();
	void ResetColor();

	void SetTeamIdentifier(sf::Int8 identifier) override;
	void SetName(const std::string& name) override;

	sf::IntRect GetSize() const;
	void Jump() override;
	void SetGrounded(Platform* platform) override;
	void SetGrounded() override;
	void MoveOutOfCollision(const sf::Time dt, const sf::FloatRect& rect);

private:
	void UpdateRay() const;
	void CreateRay();
	sf::FloatRect GetBoundingRect() const;

	sf::Text m_name_text;
	sf::Text m_team_id_text;
	SpriteComponentPtr mSpriteComponent;
	bool m_show_jump_animation{};
	/*Animation m_jump_smoke_animation;*/
};

