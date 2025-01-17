//utilities for visibility buffer

//packs vertex id and instance id into 1 uint
uint calculateOutputVIID(uint vertexId, uint instanceId)
{
	uint drawID_primID = ((vertexId << 23) & 0x7F800000) | (instanceId & 0x007FFFFF);
	return drawID_primID;
}

//unpacks vertex id and instance id
void unpackOutputVBID(uint packedID, out uint vertexId, out uint instanceId)
{
    vertexId = (packedID >> 23) & 0xFF;
    instanceId = packedID & 0x007FFFFF;
}