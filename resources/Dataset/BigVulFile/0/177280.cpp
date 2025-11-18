bool ACodec::BaseState::onOMXMessageList(const sp<AMessage> &msg) {
    sp<RefBase> obj;
    CHECK(msg->findObject("messages", &obj));
    sp<MessageList> msgList = static_cast<MessageList *>(obj.get());

 bool receivedRenderedEvents = false;
 for (std::list<sp<AMessage>>::const_iterator it = msgList->getList().cbegin();
          it != msgList->getList().cend(); ++it) {
 (*it)->setWhat(ACodec::kWhatOMXMessageItem);
        mCodec->handleMessage(*it);
 int32_t type;
        CHECK((*it)->findInt32("type", &type));
 if (type == omx_message::FRAME_RENDERED) {
            receivedRenderedEvents = true;
 }
 }

 if (receivedRenderedEvents) {
        mCodec->notifyOfRenderedFrames();
 }
 return true;
}
