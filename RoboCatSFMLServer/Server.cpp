#include "ColorJumpServerPCH.hpp"

//uncomment this when you begin working on the server

bool Server::StaticInit()
{
	s_instance.reset(new Server());

	World::StaticInit(new WorldServer());

	return true;
}

Server::Server()
{
	InitNetworkManager();

	// Setup latency
	float latency = 0.0f;
	const string latencyString = StringUtils::GetCommandLineArg(2);
	if (!latencyString.empty())
	{
		latency = stof(latencyString);
	}
	NetworkManagerServer::sInstance->SetSimulatedLatency(latency);
}


int Server::Run()
{
	SetupWorld();

	return Engine::Run();
}

bool Server::InitNetworkManager() const
{
	const string portString = StringUtils::GetCommandLineArg(1);
	const auto port = static_cast<uint16_t>(stoi(portString));

	return NetworkManagerServer::StaticInit(port);
}


void Server::SetupWorld()
{
}

void Server::DoFrame()
{
	NetworkManagerServer::sInstance->ProcessIncomingPackets();
	NetworkManagerServer::sInstance->CheckForDisconnects();

	Engine::DoFrame();

	NetworkManagerServer::sInstance->Update(Timing::sInstance.GetDeltaTime());
}
