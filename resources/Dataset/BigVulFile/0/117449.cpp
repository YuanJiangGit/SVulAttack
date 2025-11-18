void WebGraphicsContext3DCommandBufferImpl::prepareTexture() {
  if (ShouldUseSwapClient())
    swap_client_->OnViewContextSwapBuffersPosted();
  context_->SwapBuffers();
  context_->Echo(base::Bind(
      &WebGraphicsContext3DCommandBufferImpl::OnSwapBuffersComplete,
      weak_ptr_factory_.GetWeakPtr()));
#if defined(OS_MACOSX)
  gl_->Flush();
#endif
}
