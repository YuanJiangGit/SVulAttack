void OfflinePageModelImpl::AddPage(const OfflinePageItem& page,
                                   const AddPageCallback& callback) {
  RunWhenLoaded(base::Bind(&OfflinePageModelImpl::AddPageWhenLoadDone,
                           weak_ptr_factory_.GetWeakPtr(), page, callback));
}
