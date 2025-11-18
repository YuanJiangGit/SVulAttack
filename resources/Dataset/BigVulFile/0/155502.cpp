int64_t DataReductionProxySettings::GetDataReductionLastUpdateTime() {
  DCHECK(thread_checker_.CalledOnValidThread());
  DCHECK(data_reduction_proxy_service_->compression_stats());
  return
      data_reduction_proxy_service_->compression_stats()->GetLastUpdateTime();
}
