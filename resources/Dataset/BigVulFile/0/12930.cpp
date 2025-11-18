void nlmsg_set_proto(struct nl_msg *msg, int protocol)
{
	msg->nm_protocol = protocol;
}
