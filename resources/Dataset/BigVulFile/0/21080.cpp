void mem_cgroup_lru_del(struct page *page)
{
	mem_cgroup_lru_del_list(page, page_lru(page));
}
