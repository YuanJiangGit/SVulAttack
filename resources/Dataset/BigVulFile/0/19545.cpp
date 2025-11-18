struct logicalVolIntegrityDescImpUse *udf_sb_lvidiu(struct udf_sb_info *sbi)
{
	struct logicalVolIntegrityDesc *lvid =
		(struct logicalVolIntegrityDesc *)sbi->s_lvid_bh->b_data;
	__u32 number_of_partitions = le32_to_cpu(lvid->numOfPartitions);
	__u32 offset = number_of_partitions * 2 *
				sizeof(uint32_t)/sizeof(uint8_t);
	return (struct logicalVolIntegrityDescImpUse *)&(lvid->impUse[offset]);
}
