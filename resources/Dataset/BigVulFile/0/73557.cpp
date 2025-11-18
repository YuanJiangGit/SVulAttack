MagickExport void SetQuantumAlphaType(QuantumInfo *quantum_info,
  const QuantumAlphaType type)
{
  assert(quantum_info != (QuantumInfo *) NULL);
  assert(quantum_info->signature == MagickCoreSignature);
  quantum_info->alpha_type=type;
}
