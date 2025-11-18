  explicit FakeMidiManagerClient(Logger* logger)
      : complete_start_session_(false),
        result_(MIDI_NOT_SUPPORTED),
        logger_(logger) {}
