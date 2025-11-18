XRenderDepthCheckErrorHandler (Display *dpy, XErrorEvent *evt)
{
    if (evt->request_code == X_CreatePixmap && evt->error_code == BadValue)
    {
	DepthCheckPtr	d;
	_XLockMutex(_Xglobal_lock);
	for (d = depthChecks; d; d = d->next)
	    if (d->dpy == dpy)
	    {
		if ((long) (evt->serial - d->serial) >= 0)
		    d->missing |= DEPTH_MASK(evt->resourceid);
		break;
	    }
	_XUnlockMutex (_Xglobal_lock);
    }
    return 0;
}
