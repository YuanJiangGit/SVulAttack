void UiSceneCreator::CreateWebVrUrlToast() {
  auto* parent =
      AddTransientParent(kWebVrUrlToastTransientParent, kWebVrViewportAwareRoot,
                         kWebVrUrlToastTimeoutSeconds, true, scene_);
  parent->AddBinding(VR_BIND_FUNC(bool, Model, model_,
                                  web_vr_started_for_autopresentation &&
                                      !model->web_vr_show_splash_screen &&
                                      model->web_vr_has_produced_frames(),
                                  UiElement, parent, SetVisible));

  auto element = base::MakeUnique<WebVrUrlToast>(
      512, base::Bind(&UiBrowserInterface::OnUnsupportedMode,
                      base::Unretained(browser_)));
  element->SetName(kWebVrUrlToast);
  element->set_opacity_when_visible(0.8f);
  element->SetDrawPhase(kPhaseOverlayForeground);
  element->set_hit_testable(false);
  element->SetTranslate(0, kWebVrToastDistance * sin(kWebVrUrlToastRotationRad),
                        -kWebVrToastDistance * cos(kWebVrUrlToastRotationRad));
  element->SetRotate(1, 0, 0, kWebVrUrlToastRotationRad);
  element->SetSize(kWebVrUrlToastWidth, kWebVrUrlToastHeight);
  BindColor(model_, element.get(),
            &ColorScheme::web_vr_transient_toast_background,
            &TexturedElement::SetBackgroundColor);
  BindColor(model_, element.get(),
            &ColorScheme::web_vr_transient_toast_foreground,
            &TexturedElement::SetForegroundColor);
  element->AddBinding(VR_BIND_FUNC(ToolbarState, Model, model_, toolbar_state,
                                   WebVrUrlToast, element.get(),
                                   SetToolbarState));
  scene_->AddUiElement(kWebVrUrlToastTransientParent, std::move(element));
}
