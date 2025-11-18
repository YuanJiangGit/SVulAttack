 void readVector(Parcel &reply, Vector<uint8_t> &vector) const {
 uint32_t size = reply.readInt32();
 vector.insertAt((size_t)0, size);
        reply.read(vector.editArray(), size);
 }
