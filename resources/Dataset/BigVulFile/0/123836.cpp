skia::RefPtr<SkPicture> RenderViewImpl::CapturePicture() {
  return compositor_ ? compositor_->CapturePicture() :
      skia::RefPtr<SkPicture>();
}
