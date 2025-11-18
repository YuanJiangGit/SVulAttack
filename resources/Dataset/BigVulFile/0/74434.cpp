BOOLEAN ParaNdis_AnalyzeReceivedPacket(
    PVOID headersBuffer,
    ULONG dataLength,
    PNET_PACKET_INFO packetInfo)
{
    NdisZeroMemory(packetInfo, sizeof(*packetInfo));

    packetInfo->headersBuffer = headersBuffer;
    packetInfo->dataLength = dataLength;

    if(!AnalyzeL2Hdr(packetInfo))
        return FALSE;

    if (!AnalyzeL3Hdr(packetInfo))
        return FALSE;

    return TRUE;
}
