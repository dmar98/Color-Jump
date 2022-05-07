#pragma once
#include <vector>

#include "EColorType.hpp"
#include "EPlatformType.hpp"

class PlatformPart;

class Platform
{
public:
	Platform(sf::Int8 id, EPlatformType platform_type);
	virtual ~Platform() = default;
	EPlatformType GetPlatformType() const;
	virtual void AddPlatformPart(PlatformPart* tile);
	virtual bool HandlePlayerCollisionAndChangeColor(EColorType color_type);
	virtual bool HandlePlayerCollision(EColorType color_type);
	virtual void SetType(EPlatformType type);
	void ResetToInitialType();

	virtual void Update();
	sf::Int8 GetID() const;
	std::vector<PlatformPart*>& GetParts();

protected:
	EPlatformType m_type;
	EPlatformType m_initial_type;
	std::vector<PlatformPart*> m_platform_parts;
	bool m_is_pulse{};
	float m_current_pulse_cooldown;
	sf::Int8 m_id;
};

