#pragma once
#define _USE_MATH_DEFINES
#include "ColorJumpShared.hpp"

class CharacterClient;

#include "StateID.hpp"
#include "ButtonType.hpp"
#include "PlayerAction.hpp"

#include "SFML/Graphics.hpp"

#include "Component.hpp"
#include "DrawableComponent.hpp"
#include "Button.hpp"
#include "Label.hpp"
#include "Container.hpp"
#include "TextComponent.hpp"

#include "Utility.hpp"
#include "KeyBinding.hpp"

#include "Command.hpp"
#include "CommandQueue.hpp"

#include "Player.hpp"

#include "State.hpp"
#include "StateStack.hpp"

#include "InputManager.hpp"
#include "SpriteComponent.hpp"
#include "TextureManager.hpp"
#include "FontManager.hpp"
#include "AudioManager.hpp"
#include "RenderManager.hpp"
#include "WindowManager.hpp"
#include "StackManager.hpp"

#include "JoinServerState.hpp"
#include "LobbyState.hpp"
#include "MenuState.hpp"
#include "MultiPlayerGameOverState.hpp"
#include "MultiplayerGameState.hpp"
#include "SettingsState.hpp"
#include "TitleState.hpp"
#include "TutorialState.hpp"

#include "NetworkManagerClient.hpp"
#include "Client.hpp"
#include "PlayerDataManager.hpp"

#include "CharacterClient.hpp"
#include "GhostCharacterClient.hpp"

#include "LevelLoaderClient.hpp"
#include "TileClient.hpp"
#include "PlatformClient.hpp"
#include "PlatformPartClient.hpp"
#include "TileFactoryClient.hpp"
#include "RayGround.hpp"

#include "CollisionHandler.hpp"

#include "WorldClient.hpp"
