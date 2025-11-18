  explicit EventFilterForPopupExit(RenderWidgetHostViewAura* rwhva)
      : rwhva_(rwhva) {
    DCHECK(rwhva_);
    aura::Window* root_window = rwhva_->window_->GetRootWindow();
    DCHECK(root_window);
    root_window->AddPreTargetHandler(this);
  }
