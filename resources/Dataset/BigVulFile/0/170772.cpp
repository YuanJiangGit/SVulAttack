 virtual uint32_t getCaps() {
 Parcel data, reply;
        data.writeInterfaceToken(IHDCP::getInterfaceDescriptor());
        remote()->transact(HDCP_GET_CAPS, data, &reply);
 return reply.readInt32();
 }
