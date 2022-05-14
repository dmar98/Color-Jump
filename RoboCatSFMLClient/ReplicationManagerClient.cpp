#include "ColorJumpClientPCH.hpp"

void ReplicationManagerClient::Read(InputMemoryBitStream& inInputStream)
{
	while (inInputStream.GetRemainingBitCount() >= 32)
	{
		//read the network id...
		int networkId;
		inInputStream.Read(networkId);

		//only need 2 bits for action...
		uint8_t action;
		inInputStream.Read(action, 2);

		switch (static_cast<ReplicationAction>(action))
		{
		case ReplicationAction::RA_Create:
			ReadAndDoCreateAction(inInputStream, networkId);
			break;
		case ReplicationAction::RA_Update:
			ReadAndDoUpdateAction(inInputStream, networkId);
			break;
		case ReplicationAction::RA_Destroy:
			ReadAndDoDestroyAction(networkId);
			break;
		case ReplicationAction::RA_RPC:
		case ReplicationAction::RA_MAX:
		default:
			break;
		}
	}
}

void ReplicationManagerClient::ReadAndDoCreateAction(InputMemoryBitStream& inInputStream,
                                                     const int inNetworkId)
{
	//need 4 cc
	uint32_t fourCCName;
	inInputStream.Read(fourCCName);

	//we might already have this object- could happen if our ack of the create got dropped so server resend create request 
	//( even though we might have created )
	GameObjectPtr gameObject = NetworkManagerClient::sInstance->GetGameObject(inNetworkId);
	if (!gameObject)
	{
		//create the object and map it...
		gameObject = GameObjectRegistry::sInstance->CreateGameObject(fourCCName);
		gameObject->SetNetworkId(inNetworkId);
		NetworkManagerClient::sInstance->AddNetworkIdToGameObjectMap(gameObject);

		//it had really be the right type...
		assert(gameObject->GetClassId() == fourCCName);
	}

	//and read state
	gameObject->Read(inInputStream);
}

void ReplicationManagerClient::ReadAndDoUpdateAction(InputMemoryBitStream& inInputStream,
                                                     const int inNetworkId)
{
	//need object
	const GameObjectPtr gameObject = NetworkManagerClient::sInstance->GetGameObject(inNetworkId);

	//gameObject MUST be found, because create was ack'd if we're getting an update...
	//and read state
	gameObject->Read(inInputStream);
}

void ReplicationManagerClient::ReadAndDoDestroyAction(const int inNetworkId)
{
	//if something was destroyed before the create went through, we'll never get it
	//but we might get the destroy request, so be tolerant of being asked to destroy something that wasn't created
	const GameObjectPtr gameObject = NetworkManagerClient::sInstance->GetGameObject(inNetworkId);
	if (gameObject)
	{
		gameObject->SetDoesWantToDie(true);
		NetworkManagerClient::sInstance->RemoveNetworkIdToGameObjectMap(gameObject);
	}
}
