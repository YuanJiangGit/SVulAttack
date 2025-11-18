void LayerTreeCoordinator::syncLayerState(WebLayerID id, const WebLayerInfo& info)
{
    if (m_shouldSendScrollPositionUpdate) {
        m_webPage->send(Messages::LayerTreeCoordinatorProxy::DidChangeScrollPosition(m_visibleContentsRect.location()));
        m_shouldSendScrollPositionUpdate = false;
    }

    m_shouldSyncFrame = true;
    m_webPage->send(Messages::LayerTreeCoordinatorProxy::SetCompositingLayerState(id, info));
}
