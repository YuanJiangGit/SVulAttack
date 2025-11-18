PermissionPromptImpl::PermissionPromptImpl(Browser* browser, Delegate* delegate)
    : browser_(browser), delegate_(delegate), bubble_delegate_(nullptr) {
  Show();
}
