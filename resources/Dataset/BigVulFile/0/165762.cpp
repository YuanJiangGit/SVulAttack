void SVGElement::AddReferenceTo(SVGElement* target_element) {
  DCHECK(target_element);

  EnsureSVGRareData()->OutgoingReferences().insert(target_element);
  target_element->EnsureSVGRareData()->IncomingReferences().insert(this);
}
