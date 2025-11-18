FLAC__bool read_metadata_seektable_(FLAC__StreamDecoder *decoder, FLAC__bool is_last, unsigned length)
{
	FLAC__uint32 i, x;
	FLAC__uint64 xx;

	FLAC__ASSERT(FLAC__bitreader_is_consumed_byte_aligned(decoder->private_->input));

	decoder->private_->seek_table.type = FLAC__METADATA_TYPE_SEEKTABLE;
	decoder->private_->seek_table.is_last = is_last;
	decoder->private_->seek_table.length = length;

	decoder->private_->seek_table.data.seek_table.num_points = length / FLAC__STREAM_METADATA_SEEKPOINT_LENGTH;

 /* use realloc since we may pass through here several times (e.g. after seeking) */
 if(0 == (decoder->private_->seek_table.data.seek_table.points = safe_realloc_mul_2op_(decoder->private_->seek_table.data.seek_table.points, decoder->private_->seek_table.data.seek_table.num_points, /*times*/sizeof(FLAC__StreamMetadata_SeekPoint)))) {
		decoder->protected_->state = FLAC__STREAM_DECODER_MEMORY_ALLOCATION_ERROR;
 return false;
 }
 for(i = 0; i < decoder->private_->seek_table.data.seek_table.num_points; i++) {
 if(!FLAC__bitreader_read_raw_uint64(decoder->private_->input, &xx, FLAC__STREAM_METADATA_SEEKPOINT_SAMPLE_NUMBER_LEN))
 return false; /* read_callback_ sets the state for us */
		decoder->private_->seek_table.data.seek_table.points[i].sample_number = xx;

 if(!FLAC__bitreader_read_raw_uint64(decoder->private_->input, &xx, FLAC__STREAM_METADATA_SEEKPOINT_STREAM_OFFSET_LEN))
 return false; /* read_callback_ sets the state for us */
		decoder->private_->seek_table.data.seek_table.points[i].stream_offset = xx;

 if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &x, FLAC__STREAM_METADATA_SEEKPOINT_FRAME_SAMPLES_LEN))
 return false; /* read_callback_ sets the state for us */
		decoder->private_->seek_table.data.seek_table.points[i].frame_samples = x;
 }
	length -= (decoder->private_->seek_table.data.seek_table.num_points * FLAC__STREAM_METADATA_SEEKPOINT_LENGTH);
 /* if there is a partial point left, skip over it */
 if(length > 0) {
 /*@@@ do a send_error_to_client_() here?  there's an argument for either way */
 if(!FLAC__bitreader_skip_byte_block_aligned_no_crc(decoder->private_->input, length))
 return false; /* read_callback_ sets the state for us */
 }

 return true;
}
