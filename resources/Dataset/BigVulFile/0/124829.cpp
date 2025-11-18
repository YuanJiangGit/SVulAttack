LayoutUnit RenderBox::computeContentLogicalHeight(const Length& height, LayoutUnit intrinsicContentHeight) const
{
    LayoutUnit heightIncludingScrollbar = computeContentAndScrollbarLogicalHeightUsing(height, intrinsicContentHeight);
    if (heightIncludingScrollbar == -1)
        return -1;
    return std::max<LayoutUnit>(0, adjustContentBoxLogicalHeightForBoxSizing(heightIncludingScrollbar) - scrollbarLogicalHeight());
}
