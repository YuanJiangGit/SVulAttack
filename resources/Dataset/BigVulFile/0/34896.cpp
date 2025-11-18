rpc_call_start(struct rpc_task *task)
{
	task->tk_action = call_start;
}
