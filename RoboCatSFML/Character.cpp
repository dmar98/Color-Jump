//Written by Dylan Goncalves Martins (D00242562), modified by Paul Bichler (D00242563)
#include "ColorJumpPCH.hpp"

void Character::SetVelocity(const Vector3& inVelocity)
{
	m_velocity = inVelocity;
}

const Vector3& Character::GetVelocity() const
{
	return m_velocity;
}

void Character::Accelerate(const float vx, const float vy)
{
	m_velocity.mX += vx;
	m_velocity.mY += vy;
}

void Character::Accelerate(const sf::Vector2f vector2)
{
	Accelerate(vector2.x, vector2.y);
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Creates the character for the players
 *	Also Creates a "Ray" and sets it as a child of the character
 */
Character::Character(const EColorType type, const sf::IntRect& texture_rect)
	: GameObject(),
	  m_type(type),
	  m_grounded(false),
	  m_current_platform(nullptr),
	  m_can_jump(true),
	  m_velocity(0, 0, 0),
	  m_is_dead(false)
{
}

float Character::GetMaxSpeed()
{
	return 200.f;
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Jump action for the player
 *	Check if player can jump is also done here
 */
void Character::Jump()
{
	/*Utility::Debug("Jump requested");*/

	if (m_can_jump == false)
	{
		return;
	}

	m_can_jump = false;
	m_grounded = false;
	m_current_platform = nullptr;
	SetVelocity(Vector3(0, -500.f, 0));
	LOG("Jump", 0)
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	This is called when the player lands on a platform
 *	Removes y axis velocity and moves character slightly up
 */
void Character::SetGrounded(Platform* platform)
{
	m_can_jump = true;
	m_grounded = true;
	m_current_platform = platform;
	SetVelocity(Vector3(m_velocity.mX, 0, 0));
	SetLocation(Vector3(GetLocation().mX, GetLocation().mY - 2, 0));
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	This is called when the Ray has no platform with which he collides
 */
void Character::SetFalling()
{
	m_grounded = false;
	m_current_platform = nullptr;
}

EColorType Character::GetCharacterType() const
{
	return m_type;
}

Platform* Character::GetCurrentPlatform() const
{
	return m_current_platform;
}

bool Character::IsOnPlatformOfType(const EPlatformType platform_type) const
{
	if (m_current_platform == nullptr)
		return false;

	return m_current_platform->GetPlatformType() == platform_type;
}

bool Character::IsOnPlatform(Platform* platform) const
{
	return m_current_platform == platform;
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Removes velocity on x axis and sets y axis to gravity force
 */
void Character::StopMovement()
{
	SetVelocity(Vector3(0, 9.81f, 0));
}

void Character::SetIsDead(bool isDead)
{
	m_is_dead = isDead;
}

bool Character::IsDead() const
{
	return m_is_dead;
}

void Character::SetIdentifier(const int identifier)
{
	m_identifier = identifier;
}

void Character::SetTeamIdentifier(const int identifier)
{
	m_team_identifier = identifier;
}

void Character::SetName(const std::string& name)
{
	m_name = name;
}

int Character::GetPlayerID() const
{
	return m_identifier;
}

int Character::GetTeamIdentifier() const
{
	return m_team_identifier;
}

std::string Character::GetName() const
{
	return m_name;
}

unsigned Character::GetCategory() const
{
	if (m_type == kRed)
	{
		return static_cast<int>(Category::kPlayerRed);
	}
	return static_cast<int>(Category::kPlayerBlue);
}

void Character::Update()
{
	SetLocation(GetLocation() + m_velocity * Timing::sInstance.GetDeltaTime() * 2);
	Accelerate(-m_velocity.mX * 0.75f, 0);
	if (!m_grounded)
	{
		Accelerate(0, 9.81f * 2);
	}
}
