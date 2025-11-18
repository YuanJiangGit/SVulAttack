asn1_der_decoding (asn1_node * element, const void *ider, int ider_len,
		   char *errorDescription)
{
  return asn1_der_decoding2 (element, ider, &ider_len, 0, errorDescription);
}
