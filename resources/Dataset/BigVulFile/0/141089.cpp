  int GetOutliveTimeCount() const {
    if (!gc_age_when_document_detached_)
      return 0;
    return ThreadState::Current()->GcAge() - gc_age_when_document_detached_;
  }
