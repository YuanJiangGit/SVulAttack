void DocumentLoader::addArchiveResource(PassRefPtr<ArchiveResource> resource)
{
    if (!m_archiveResourceCollection)
        m_archiveResourceCollection = adoptPtr(new ArchiveResourceCollection);
        
    ASSERT(resource);
    if (!resource)
        return;
        
    m_archiveResourceCollection->addResource(resource);
}
