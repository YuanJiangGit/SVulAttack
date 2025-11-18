void ImageResource::DestroyDecodedDataForFailedRevalidation() {
  UpdateImage(nullptr, ImageResourceContent::kClearAndUpdateImage, false);
  SetDecodedSize(0);
}
