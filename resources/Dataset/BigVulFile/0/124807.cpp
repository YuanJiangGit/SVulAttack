bool RenderBox::avoidsFloats() const
{
    return isReplaced() || hasOverflowClip() || isHR() || isLegend() || isWritingModeRoot() || isFlexItemIncludingDeprecated();
}
