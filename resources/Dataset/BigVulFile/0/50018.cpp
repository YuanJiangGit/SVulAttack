archive_read_has_encrypted_entries(struct archive *_a)
{
	struct archive_read *a = (struct archive_read *)_a;
	int format_supports_encryption = archive_read_format_capabilities(_a)
			& (ARCHIVE_READ_FORMAT_CAPS_ENCRYPT_DATA | ARCHIVE_READ_FORMAT_CAPS_ENCRYPT_METADATA);

	if (!_a || !format_supports_encryption) {
		/* Format in general doesn't support encryption */
		return ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED;
	}

	/* A reader potentially has read enough data now. */
	if (a->format && a->format->has_encrypted_entries) {
		return (a->format->has_encrypted_entries)(a);
	}

	/* For any other reason we cannot say how many entries are there. */
	return ARCHIVE_READ_FORMAT_ENCRYPTION_DONT_KNOW;
}
