//utilities for visibility buffer

//packs vertex id and instance id into 1 uint
uint calculateOutputVIID(uint vertexId, uint instanceId)
{
	uint drawID_primID = ((instanceId << 23) & 0x7F800000) | (vertexId & 0x007FFFFF);
	return drawID_primID;
}

//unpacks vertex id and instance id
void unpackOutputVBID(uint packedID, out uint vertexId, out uint instanceId)
{
    instanceId = (packedID >> 23) & 0xFF;
    vertexId = packedID & 0x007FFFFF;
}