#pragma once

namespace Category
{
	enum Type
	{
		kNone = 0,
		kScene = 1 << 0,
		kGhost = 1 << 1,
		kParticleSystem = 1 << 2,
		kSoundEffect = 1 << 3,
		kPlayerRed = 1 << 4,
		kPlayerBlue = 1 << 5,
		kPlatform = 1 << 6,
		kRayRedPlayer = 1 << 9,
		kRayBluePlayer = 1 << 10,
		kTrap = 1 << 11,

		kPlayer = kPlayerRed | kPlayerBlue,
		kRay = kRayRedPlayer | kRayBluePlayer,
	};
}
