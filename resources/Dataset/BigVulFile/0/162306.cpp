SharedMemoryHandleProvider::GetHandleForInProcessAccess() {
  {
    base::AutoLock lock(mapping_lock_);
#if DCHECK_IS_ON()
    DCHECK_GE(map_ref_count_, 0);
    ++map_ref_count_;
#endif
    if (!shared_memory_->memory()) {
      CHECK(shared_memory_->Map(mapped_size_));
      DVLOG(3) << __func__ << ": Mapped memory for in-process access @"
               << shared_memory_->memory() << '.';
    }
  }

  return std::make_unique<Handle>(this);
}
