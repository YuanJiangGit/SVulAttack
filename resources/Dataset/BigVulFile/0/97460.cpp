String FrameLoader::outgoingOrigin() const
{
    return m_frame->document()->securityOrigin()->toString();
}
