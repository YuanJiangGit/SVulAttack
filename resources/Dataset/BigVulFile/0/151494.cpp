void WorkerFetchContext::DispatchDidReceiveEncodedData(
    unsigned long identifier,
    int encoded_data_length) {
  probe::didReceiveEncodedDataLength(global_scope_, identifier,
                                     encoded_data_length);
}
