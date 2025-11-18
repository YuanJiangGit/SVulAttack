  void ConnectPeerTransport(FakePacketTransport* peer_packet_transport) {
    DCHECK(!peer_packet_transport_);
    peer_packet_transport_ = peer_packet_transport;
  }
