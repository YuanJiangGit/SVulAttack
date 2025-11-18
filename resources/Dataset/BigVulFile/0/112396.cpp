void Document::buildAccessKeyMap(TreeScope* scope)
{
    ASSERT(scope);
    Node* rootNode = scope->rootNode();
    for (Element* element = ElementTraversal::firstWithin(rootNode); element; element = ElementTraversal::next(element, rootNode)) {
        const AtomicString& accessKey = element->getAttribute(accesskeyAttr);
        if (!accessKey.isEmpty())
            m_elementsByAccessKey.set(accessKey.impl(), element);

        for (ShadowRoot* root = element->youngestShadowRoot(); root; root = root->olderShadowRoot())
            buildAccessKeyMap(root);
    }
}
