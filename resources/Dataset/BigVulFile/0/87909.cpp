static bool fm10k_add_rx_frag(struct fm10k_rx_buffer *rx_buffer,
			      unsigned int size,
			      union fm10k_rx_desc *rx_desc,
			      struct sk_buff *skb)
{
	struct page *page = rx_buffer->page;
	unsigned char *va = page_address(page) + rx_buffer->page_offset;
#if (PAGE_SIZE < 8192)
	unsigned int truesize = FM10K_RX_BUFSZ;
#else
	unsigned int truesize = ALIGN(size, 512);
#endif
	unsigned int pull_len;

	if (unlikely(skb_is_nonlinear(skb)))
		goto add_tail_frag;

	if (likely(size <= FM10K_RX_HDR_LEN)) {
		memcpy(__skb_put(skb, size), va, ALIGN(size, sizeof(long)));

		/* page is not reserved, we can reuse buffer as-is */
		if (likely(!fm10k_page_is_reserved(page)))
			return true;

		/* this page cannot be reused so discard it */
		__free_page(page);
		return false;
	}

	/* we need the header to contain the greater of either ETH_HLEN or
	 * 60 bytes if the skb->len is less than 60 for skb_pad.
	 */
	pull_len = eth_get_headlen(va, FM10K_RX_HDR_LEN);

	/* align pull length to size of long to optimize memcpy performance */
	memcpy(__skb_put(skb, pull_len), va, ALIGN(pull_len, sizeof(long)));

	/* update all of the pointers */
	va += pull_len;
	size -= pull_len;

add_tail_frag:
	skb_add_rx_frag(skb, skb_shinfo(skb)->nr_frags, page,
			(unsigned long)va & ~PAGE_MASK, size, truesize);

	return fm10k_can_reuse_rx_page(rx_buffer, page, truesize);
}
