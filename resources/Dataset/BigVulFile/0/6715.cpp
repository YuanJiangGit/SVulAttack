static void ide_cd_eject_request_cb(void *opaque, bool force)
{
    IDEState *s = opaque;

    s->events.eject_request = true;
    if (force) {
        s->tray_locked = false;
    }
    ide_set_irq(s->bus);
}
