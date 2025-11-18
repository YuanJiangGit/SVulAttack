static uint32_t timer_is_periodic(HPETTimer *t)
{
    return t->config & HPET_TN_PERIODIC;
}
