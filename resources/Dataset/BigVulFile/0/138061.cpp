bool AXNodeObject::isEmbeddedObject() const {
  return isHTMLPlugInElement(getNode());
}
