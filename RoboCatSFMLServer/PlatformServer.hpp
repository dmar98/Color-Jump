#pragma once

#include "EColorType.hpp"
#include "EPlatformType.hpp"

class PlatformPart;

class PlatformServer : Platform
{
public:
	PlatformServer(sf::Int8 id, EPlatformType platform_type);
	bool HandlePlayerCollisionAndChangeColor(EColorType color_type) override;
	bool HandlePlayerCollision(EColorType color_type) override;
	void SetType(EPlatformType type) override;

	void Update() override;
};

