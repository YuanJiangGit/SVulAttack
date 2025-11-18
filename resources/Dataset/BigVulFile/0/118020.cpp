void V8Proxy::setIsolatedWorldSecurityOrigin(int worldID, PassRefPtr<SecurityOrigin> prpSecurityOriginIn)
{
    ASSERT(worldID);
    RefPtr<SecurityOrigin> securityOrigin = prpSecurityOriginIn;
    m_isolatedWorldSecurityOrigins.set(worldID, securityOrigin);
    IsolatedWorldMap::iterator iter = m_isolatedWorlds.find(worldID);
    if (iter != m_isolatedWorlds.end())
        iter->second->setSecurityOrigin(securityOrigin);
}
