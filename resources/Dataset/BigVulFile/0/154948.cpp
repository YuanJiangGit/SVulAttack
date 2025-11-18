void WebGLRenderingContextBase::ForceRestoreContext() {
  if (!isContextLost()) {
    SynthesizeGLError(GL_INVALID_OPERATION, "restoreContext",
                      "context not lost");
    return;
  }

  if (!restore_allowed_) {
    if (context_lost_mode_ == kWebGLLoseContextLostContext)
      SynthesizeGLError(GL_INVALID_OPERATION, "restoreContext",
                        "context restoration not allowed");
    return;
  }

  if (!restore_timer_.IsActive())
    restore_timer_.StartOneShot(TimeDelta(), FROM_HERE);
}
