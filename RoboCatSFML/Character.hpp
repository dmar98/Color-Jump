#pragma once

class RayGround;

class Character : public GameObject
{
public:
	CLASS_IDENTIFICATION('CHAR', GameObject)

	enum ECharacterReplicationState
	{
		ECRS_Pose = 1 << 0,
		ECRS_Color = 1 << 1,
		ECRS_PlayerId = 1 << 2,

		ECRS_AllState = ECRS_Pose | ECRS_Color | ECRS_PlayerId
	};

	static GameObject* StaticCreate(const EColorType type, const sf::IntRect& texture_rect)
	{
		return new Character(type, texture_rect);
	}

	uint32_t GetAllStateMask() const override { return ECRS_AllState; }
	void SetVelocity(const Vector3& inVelocity);
	const Vector3& GetVelocity() const;
	void Accelerate(float vx, float vy);
	void Accelerate(sf::Vector2f vector2);

	void SetIdentifier(int identifier);
	virtual void SetTeamIdentifier(int identifier);
	virtual void SetName(const std::string& name);
	int GetPlayerID() const;
	int GetTeamIdentifier() const;
	std::string GetName() const;
	unsigned GetCategory() const override;

	uint32_t Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const override;


	static float GetMaxSpeed();
	virtual void Jump();
	virtual void SetGrounded(Platform* platform);
	void SetFalling();
	EColorType GetCharacterType() const;
	Platform* GetCurrentPlatform() const;
	bool IsOnPlatformOfType(EPlatformType platform_type) const;
	bool IsOnPlatform(Platform* platform) const;
	void StopMovement();
	void SetIsDead(bool isDead);
	bool IsDead() const;

protected:
	Character(EColorType type, const sf::IntRect& texture_rect);
	void Update() override;

	EColorType m_type;
	std::string m_name;
	bool m_grounded;
	Platform* m_current_platform;
	RayGround* m_ray{};
	bool m_can_jump;

	int m_identifier{};
	int m_team_identifier{};

	Vector3 m_velocity;
	bool m_is_dead;
};
