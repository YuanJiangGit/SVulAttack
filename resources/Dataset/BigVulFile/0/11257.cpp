static void read_guest_rom_state(VAPICROMState *s)
{
    cpu_physical_memory_read(s->rom_state_paddr, &s->rom_state,
                             sizeof(GuestROMState));
}
