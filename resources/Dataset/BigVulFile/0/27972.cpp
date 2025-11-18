static inline struct shmid_kernel *shm_lock(struct ipc_namespace *ns, int id)
{
	struct kern_ipc_perm *ipcp = ipc_lock(&shm_ids(ns), id);

	if (IS_ERR(ipcp))
		return (struct shmid_kernel *)ipcp;

	return container_of(ipcp, struct shmid_kernel, shm_perm);
}
