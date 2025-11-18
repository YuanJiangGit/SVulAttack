void DocumentThreadableLoader::redirectReceived(Resource* resource, ResourceRequest& request, const ResourceResponse& redirectResponse)
{
    ASSERT(m_client);
    ASSERT_UNUSED(resource, resource == this->resource());
    ASSERT(m_async);

    if (!m_actualRequest.isNull()) {
        reportResponseReceived(resource->identifier(), redirectResponse);

        handlePreflightFailure(redirectResponse.url().getString(), "Response for preflight is invalid (redirect)");

        request = ResourceRequest();

        return;
    }

    if (m_redirectMode == WebURLRequest::FetchRedirectModeManual) {
        WeakPtr<DocumentThreadableLoader> self(m_weakFactory.createWeakPtr());

        ASSERT(request.useStreamOnResponse());
        responseReceived(resource, redirectResponse, adoptPtr(new EmptyDataHandle()));

        if (!self) {
            request = ResourceRequest();
            return;
        }

        if (m_client) {
            ASSERT(m_actualRequest.isNull());
            notifyFinished(resource);
        }

        request = ResourceRequest();

        return;
    }

    if (m_redirectMode == WebURLRequest::FetchRedirectModeError) {
        ThreadableLoaderClient* client = m_client;
        clear();
        client->didFailRedirectCheck();

        request = ResourceRequest();

        return;
    }

    if (isAllowedRedirect(request.url())) {
        if (m_client->isDocumentThreadableLoaderClient())
            static_cast<DocumentThreadableLoaderClient*>(m_client)->willFollowRedirect(request, redirectResponse);
        return;
    }

    if (m_corsRedirectLimit <= 0) {
        ThreadableLoaderClient* client = m_client;
        clear();
        client->didFailRedirectCheck();
    } else if (m_options.crossOriginRequestPolicy == UseAccessControl) {
        --m_corsRedirectLimit;

        InspectorInstrumentation::didReceiveCORSRedirectResponse(document().frame(), resource->identifier(), document().frame()->loader().documentLoader(), redirectResponse, 0);

        bool allowRedirect = false;
        String accessControlErrorDescription;

        if (m_crossOriginNonSimpleRequest) {
            accessControlErrorDescription = "The request was redirected to '"+ request.url().getString() + "', which is disallowed for cross-origin requests that require preflight.";
        } else {
            allowRedirect = CrossOriginAccessControl::isLegalRedirectLocation(request.url(), accessControlErrorDescription)
                && (m_sameOriginRequest || passesAccessControlCheck(redirectResponse, effectiveAllowCredentials(), getSecurityOrigin(), accessControlErrorDescription, m_requestContext));
        }

        if (allowRedirect) {
            clearResource();

            RefPtr<SecurityOrigin> originalOrigin = SecurityOrigin::create(redirectResponse.url());
            RefPtr<SecurityOrigin> requestOrigin = SecurityOrigin::create(request.url());
            if (!m_sameOriginRequest && !originalOrigin->isSameSchemeHostPort(requestOrigin.get()))
                m_securityOrigin = SecurityOrigin::createUnique();
            m_sameOriginRequest = false;

            if (m_resourceLoaderOptions.credentialsRequested == ClientDidNotRequestCredentials)
                m_forceDoNotAllowStoredCredentials = true;

            request.clearHTTPReferrer();
            request.clearHTTPOrigin();
            request.clearHTTPUserAgent();
            for (const auto& header : m_simpleRequestHeaders)
                request.setHTTPHeaderField(header.key, header.value);
            makeCrossOriginAccessRequest(request);
            return;
        }

        ThreadableLoaderClient* client = m_client;
        clear();
        client->didFailAccessControlCheck(ResourceError(errorDomainBlinkInternal, 0, redirectResponse.url().getString(), accessControlErrorDescription));
    } else {
        ThreadableLoaderClient* client = m_client;
        clear();
        client->didFailRedirectCheck();
    }

    request = ResourceRequest();
}
