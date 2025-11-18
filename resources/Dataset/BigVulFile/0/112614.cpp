void Document::visualUpdatesSuppressionTimerFired(Timer<Document>*)
{
    ASSERT(!m_visualUpdatesAllowed);
    setVisualUpdatesAllowed(true);
}
