create_filegen_node(
	int		filegen_token,
	attr_val_fifo *	options
	)
{
	filegen_node *my_node;
	
	my_node = emalloc_zero(sizeof(*my_node));
	my_node->filegen_token = filegen_token;
	my_node->options = options;

	return my_node;
}
