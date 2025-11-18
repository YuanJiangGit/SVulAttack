char16_t* strstr16(const char16_t* src, const char16_t* target)
{
 const char16_t needle = *target++;
 const size_t target_len = strlen16(target);
 if (needle != '\0') {
 do {
 do {
 if (*src == '\0') {
 return nullptr;
 }
 } while (*src++ != needle);
 } while (strncmp16(src, target, target_len) != 0);
      src--;
 }

 return (char16_t*)src;
}
