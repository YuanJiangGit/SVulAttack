u32 NextPacket(u8 **pStrm)
{

    u32 index;
    u32 maxIndex;
    u32 zeroCount;
    u8 *stream;
    u8 byte;
 static u32 prevIndex=0;

 /* For default stream mode all the stream is in first packet */
 if (!packetize && !nalUnitStream)
 return 0;

    index = 0;
    stream = *pStrm + prevIndex;
    maxIndex = (u32)(streamStop - stream);

 if (maxIndex == 0)
 return(0);

 /* leading zeros of first NAL unit */
 do
 {
        byte = stream[index++];
 } while (byte != 1 && index < maxIndex);

 /* invalid start code prefix */
 if (index == maxIndex || index < 3)
 {
        DEBUG(("INVALID BYTE STREAM\n"));
        exit(100);
 }

 /* nalUnitStream is without start code prefix */
 if (nalUnitStream)
 {
        stream += index;
        maxIndex -= index;
        index = 0;
 }

    zeroCount = 0;

 /* Search stream for next start code prefix */
 /*lint -e(716) while(1) used consciously */
 while (1)
 {
        byte = stream[index++];
 if (!byte)
            zeroCount++;

 if ( (byte == 0x01) && (zeroCount >= 2) )
 {
 /* Start code prefix has two zeros
             * Third zero is assumed to be leading zero of next packet
             * Fourth and more zeros are assumed to be trailing zeros of this
             * packet */
 if (zeroCount > 3)
 {
                index -= 4;
                zeroCount -= 3;
 }
 else
 {
                index -= zeroCount+1;
                zeroCount = 0;
 }
 break;
 }
 else if (byte)
            zeroCount = 0;

 if (index == maxIndex)
 {
 break;
 }

 }

 /* Store pointer to the beginning of the packet */
 *pStrm = stream;
    prevIndex = index;

 /* nalUnitStream is without trailing zeros */
 if (nalUnitStream)
        index -= zeroCount;

 return(index);

}
