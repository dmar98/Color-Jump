#pragma once
class ReplicationManagerServer
{
public:
	void ReplicateCreate(int inNetworkId, uint32_t inInitialDirtyState);
	void ReplicateDestroy(int inNetworkId);
	void SetStateDirty(int inNetworkId, uint32_t inDirtyState);
	void HandleCreateAckd(int inNetworkId);
	void RemoveFromReplication(int inNetworkId);


private:
	uint32_t WriteDestroyAction(OutputMemoryBitStream& inOutputStream, int inNetworkId,
	                            uint32_t inDirtyState);
	unordered_map<int, ReplicationCommand> mNetworkIdToReplicationCommand;
};
