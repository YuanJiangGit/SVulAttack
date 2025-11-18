static IntPoint documentPointForWindowPoint(Frame* frame, const IntPoint& windowPoint)
{
    FrameView* view = frame->view();
    return view ? view->windowToContents(windowPoint) : windowPoint;
}
