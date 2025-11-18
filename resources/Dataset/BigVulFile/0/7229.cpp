void PolkitResultEventLoop::requestQuit(const PolkitQt1::Authority::Result& result)
{
    m_result = result;
    quit();
}
