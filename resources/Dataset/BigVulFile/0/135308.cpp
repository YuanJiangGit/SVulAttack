bool Document::needsFullLayoutTreeUpdate() const
{
    if (!isActive() || !view())
        return false;
    if (!m_useElementsNeedingUpdate.isEmpty())
        return true;
    if (!m_layerUpdateSVGFilterElements.isEmpty())
        return true;
    if (needsStyleRecalc())
        return true;
    if (needsStyleInvalidation())
        return true;
    if (childNeedsDistributionRecalc())
        return true;
    if (DocumentAnimations::needsAnimationTimingUpdate(*this))
        return true;
    return false;
}
