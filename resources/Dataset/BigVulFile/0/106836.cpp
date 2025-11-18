LayoutUnit RenderBox::computeContentBoxLogicalHeight(LayoutUnit height) const
{
    if (style()->boxSizing() == BORDER_BOX)
        height -= borderAndPaddingLogicalHeight();
    return max(0, height);
}
