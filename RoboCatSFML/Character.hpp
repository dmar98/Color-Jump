#pragma once

//#include "Animation.hpp"
#include "EColorType.hpp"
//#include "Entity.hpp"

#include <SFML/System/Time.hpp>
#include "Platform.hpp"
//#include "RayGround.hpp"

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

	static	GameObject* StaticCreate(EColorType type, const sf::IntRect& texture_rect) { return new Character(type, texture_rect); }

	uint32_t GetAllStateMask()	const override { return ECRS_AllState; }
	void SetVelocity(const Vector3& inVelocity);
	const Vector3& GetVelocity() const;
	void Accelerate(float vx, float vy);
	void ProcessInput(float inDeltaTime, const InputState& inInputState);

	void SetIdentifier(sf::Int8 identifier);
	virtual void SetTeamIdentifier(sf::Int8 identifier);
	virtual void SetName(const std::string& name);
	sf::Int8 GetIdentifier() const;
	sf::Int8 GetTeamIdentifier() const;
	std::string GetName() const;

	uint32_t Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const override;


	static float GetMaxSpeed();
	void Debug();
	virtual void Jump();
	virtual void SetGrounded(Platform* platform);
	virtual void SetGrounded();
	void SetFalling();
	EColorType GetCharacterType() const;
	Platform* GetCurrentPlatform() const;
	bool IsOnPlatformOfType(EPlatformType platform_type) const;
	bool IsOnPlatform(Platform* platform) const;
	void StopMovement();

protected:
	Character(EColorType type, const sf::IntRect& texture_rect);
	void Update() override;

private:
	/*void DrawCurrent(sf::RenderTarget&, sf::RenderStates states) const override;*/

protected:
	EColorType m_type;
	std::string m_name;
	/*sf::Sprite m_sprite;*/
	bool m_grounded;
	Platform* m_current_platform;
	RayGround* m_ray{};
	bool m_can_jump;

	/*SoundPlayer& m_sounds;*/
	sf::Int8 m_identifier{};
	sf::Int8 m_team_identifier{};

	Vector3 m_velocity;
	
};
