bool StyleResolver::checkRegionStyle(Element* regionElement)
{
    if (ScopedStyleResolver* scopedResolver = m_styleTree.scopedStyleResolverForDocument()) {
        if (scopedResolver->checkRegionStyle(regionElement))
            return true;
    }
    return false;
}
