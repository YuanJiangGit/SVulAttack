std::unique_ptr<T> Create(UiElementName name, DrawPhase phase, Args&&... args) {
  auto element = base::MakeUnique<T>(std::forward<Args>(args)...);
  element->SetName(name);
  element->SetDrawPhase(phase);
  return element;
}
