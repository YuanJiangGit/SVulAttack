static int skcipher_next_copy(struct skcipher_walk *walk)
{
	struct skcipher_walk_buffer *p;
	u8 *tmp = walk->page;

	skcipher_map_src(walk);
	memcpy(tmp, walk->src.virt.addr, walk->nbytes);
	skcipher_unmap_src(walk);

	walk->src.virt.addr = tmp;
	walk->dst.virt.addr = tmp;

	if (!(walk->flags & SKCIPHER_WALK_PHYS))
		return 0;

	p = kmalloc(sizeof(*p), skcipher_walk_gfp(walk));
	if (!p)
		return -ENOMEM;

	p->data = walk->page;
	p->len = walk->nbytes;
	skcipher_queue_write(walk, p);

	if (offset_in_page(walk->page) + walk->nbytes + walk->stride >
	    PAGE_SIZE)
		walk->page = NULL;
	else
		walk->page += walk->nbytes;

	return 0;
}
