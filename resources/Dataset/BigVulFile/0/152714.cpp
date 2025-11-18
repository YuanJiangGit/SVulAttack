HistogramBase* Histogram::FactoryTimeGet(const char* name,
                                         TimeDelta minimum,
                                         TimeDelta maximum,
                                         uint32_t bucket_count,
                                         int32_t flags) {
  return FactoryTimeGet(std::string(name), minimum, maximum, bucket_count,
                        flags);
}
