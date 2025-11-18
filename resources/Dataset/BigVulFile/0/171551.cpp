String8::String8(const char16_t* o, size_t len)
 : mString(allocFromUTF16(o, len))
{
}
