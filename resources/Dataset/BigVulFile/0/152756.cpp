void Histogram::WriteAsciiHeader(const SampleVector& samples,
                                 Count sample_count,
                                 std::string* output) const {
  StringAppendF(output,
                "Histogram: %s recorded %d samples",
                histogram_name().c_str(),
                sample_count);
  if (sample_count == 0) {
    DCHECK_EQ(samples.sum(), 0);
  } else {
    double mean = static_cast<float>(samples.sum()) / sample_count;
    StringAppendF(output, ", mean = %.1f", mean);
  }
  if (flags())
    StringAppendF(output, " (flags = 0x%x)", flags());
}
