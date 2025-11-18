void WebPagePrivate::didResumeLoading()
{
    if (!m_deferredTasks.isEmpty())
        m_deferredTasksTimer.startOneShot(0);
    m_client->didResumeLoading();
}
