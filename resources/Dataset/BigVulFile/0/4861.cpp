NewCurrentScreen(DeviceIntPtr pDev, ScreenPtr newScreen, int x, int y)
{
    DeviceIntPtr ptr;
    SpritePtr pSprite;

    ptr =
        IsFloating(pDev) ? pDev :
        GetXTestDevice(GetMaster(pDev, MASTER_POINTER));
    pSprite = ptr->spriteInfo->sprite;

    pSprite->hotPhys.x = x;
    pSprite->hotPhys.y = y;
#ifdef PANORAMIX
    if (!noPanoramiXExtension) {
        pSprite->hotPhys.x += newScreen->x - screenInfo.screens[0]->x;
        pSprite->hotPhys.y += newScreen->y - screenInfo.screens[0]->y;
        if (newScreen != pSprite->screen) {
            pSprite->screen = newScreen;
            /* Make sure we tell the DDX to update its copy of the screen */
            if (pSprite->confineWin)
                XineramaConfineCursorToWindow(ptr, pSprite->confineWin, TRUE);
            else
                XineramaConfineCursorToWindow(ptr, screenInfo.screens[0]->root,
                                              TRUE);
            /* if the pointer wasn't confined, the DDX won't get
               told of the pointer warp so we reposition it here */
            if (!syncEvents.playingEvents)
                (*pSprite->screen->SetCursorPosition) (ptr,
                                                       pSprite->screen,
                                                       pSprite->hotPhys.x +
                                                       screenInfo.screens[0]->
                                                       x - pSprite->screen->x,
                                                       pSprite->hotPhys.y +
                                                       screenInfo.screens[0]->
                                                       y - pSprite->screen->y,
                                                       FALSE);
        }
    }
    else
#endif
    if (newScreen != pSprite->hotPhys.pScreen)
        ConfineCursorToWindow(ptr, newScreen->root, TRUE, FALSE);
}
