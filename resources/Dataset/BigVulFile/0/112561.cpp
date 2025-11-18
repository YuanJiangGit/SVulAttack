void Document::setFullScreenRendererBackgroundColor(Color backgroundColor)
{
    if (!m_fullScreenRenderer)
        return;
    
    RefPtr<RenderStyle> newStyle = RenderStyle::clone(m_fullScreenRenderer->style());
    newStyle->setBackgroundColor(backgroundColor);
    m_fullScreenRenderer->setStyle(newStyle);
}
