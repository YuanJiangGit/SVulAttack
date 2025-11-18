void HTMLInputElement::resetListAttributeTargetObserver()
{
    if (inDocument())
        m_listAttributeTargetObserver = ListAttributeTargetObserver::create(fastGetAttribute(listAttr), this);
    else
        m_listAttributeTargetObserver = nullptr;
}
