bool ShouldServiceRequest(ProcessType process_type,
                          int child_id,
                          const ResourceHostMsg_Request& request_data)  {
  if (process_type == PROCESS_TYPE_PLUGIN)
    return true;

  ChildProcessSecurityPolicyImpl* policy =
      ChildProcessSecurityPolicyImpl::GetInstance();

  if (!policy->CanRequestURL(child_id, request_data.url)) {
    VLOG(1) << "Denied unauthorized request for "
            << request_data.url.possibly_invalid_spec();
    return false;
  }

  if (request_data.upload_data) {
    const std::vector<net::UploadData::Element>* uploads =
        request_data.upload_data->elements();
    std::vector<net::UploadData::Element>::const_iterator iter;
    for (iter = uploads->begin(); iter != uploads->end(); ++iter) {
      if (iter->type() == net::UploadData::TYPE_FILE &&
          !policy->CanReadFile(child_id, iter->file_path())) {
        NOTREACHED() << "Denied unauthorized upload of "
                     << iter->file_path().value();
        return false;
      }
    }
  }

  return true;
}
