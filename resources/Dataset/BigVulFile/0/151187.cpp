void InspectorNetworkAgent::GetResponseBodyBlob(
    const String& request_id,
    std::unique_ptr<GetResponseBodyCallback> callback) {
  NetworkResourcesData::ResourceData const* resource_data =
      resources_data_->Data(request_id);
  BlobDataHandle* blob = resource_data->DownloadedFileBlob();
  InspectorFileReaderLoaderClient* client = new InspectorFileReaderLoaderClient(
      blob, resource_data->MimeType(), resource_data->TextEncodingName(),
      std::move(callback));
  if (worker_global_scope_) {
    client->Start(worker_global_scope_);
    return;
  }
  DCHECK(inspected_frames_);
  LocalFrame* frame = IdentifiersFactory::FrameById(inspected_frames_,
                                                    resource_data->FrameId());
  Document* document = frame->GetDocument();
  client->Start(document);
}
