#include "ColorJumpServerPCH.hpp"

void ReplicationManagerServer::ReplicateCreate(int inNetworkId, uint32_t inInitialDirtyState)
{
	mNetworkIdToReplicationCommand[inNetworkId] = ReplicationCommand(inInitialDirtyState);
}

void ReplicationManagerServer::ReplicateDestroy(int inNetworkId)
{
	//it's broken if we don't find it...
	mNetworkIdToReplicationCommand[inNetworkId].SetDestroy();
}

void ReplicationManagerServer::RemoveFromReplication(int inNetworkId)
{
	mNetworkIdToReplicationCommand.erase(inNetworkId);
}

void ReplicationManagerServer::SetStateDirty(int inNetworkId, uint32_t inDirtyState)
{
	mNetworkIdToReplicationCommand[inNetworkId].AddDirtyState(inDirtyState);
}

void ReplicationManagerServer::HandleCreateAckd(int inNetworkId)
{
	mNetworkIdToReplicationCommand[inNetworkId].HandleCreateAckd();
}

uint32_t ReplicationManagerServer::WriteDestroyAction(OutputMemoryBitStream& inOutputStream, int inNetworkId, uint32_t inDirtyState)
{
	(void)inOutputStream;
	(void)inNetworkId;
	(void)inDirtyState;
	//don't have to do anything- action already written

	return inDirtyState;
}
