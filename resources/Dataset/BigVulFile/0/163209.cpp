void NavigationControllerImpl::RendererDidNavigateToSamePage(
    RenderFrameHostImpl* rfh,
    const FrameHostMsg_DidCommitProvisionalLoad_Params& params,
    NavigationHandleImpl* handle) {
  NavigationEntryImpl* existing_entry = GetLastCommittedEntry();

  CHECK_EQ(existing_entry->site_instance(), rfh->GetSiteInstance());

  DCHECK_EQ(pending_entry_->GetUniqueID(), params.nav_entry_id);
  existing_entry->set_unique_id(pending_entry_->GetUniqueID());

  existing_entry->set_page_type(params.url_is_unreachable ? PAGE_TYPE_ERROR
                                                          : PAGE_TYPE_NORMAL);
  if (existing_entry->update_virtual_url_with_url())
    UpdateVirtualURLToURL(existing_entry, params.url);
  existing_entry->SetURL(params.url);

  existing_entry->GetSSL() = handle->ssl_status();

  if (existing_entry->GetURL().SchemeIs(url::kHttpsScheme) &&
      !rfh->GetParent() && handle->GetNetErrorCode() == net::OK) {
    UMA_HISTOGRAM_BOOLEAN("Navigation.SecureSchemeHasSSLStatus.SamePage",
                          !!existing_entry->GetSSL().certificate);
  }

  existing_entry->set_extra_headers(pending_entry_->extra_headers());

  existing_entry->AddOrUpdateFrameEntry(
      rfh->frame_tree_node(), params.item_sequence_number,
      params.document_sequence_number, rfh->GetSiteInstance(), nullptr,
      params.url, params.referrer, params.redirects, params.page_state,
      params.method, params.post_id);

  DiscardNonCommittedEntries();
}
