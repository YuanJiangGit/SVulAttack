void mlx5_ib_free_bfreg(struct mlx5_ib_dev *dev, struct mlx5_bfreg_info *bfregi, int bfregn)
{
	mutex_lock(&bfregi->lock);
	bfregi->count[bfregn]--;
	mutex_unlock(&bfregi->lock);
}
