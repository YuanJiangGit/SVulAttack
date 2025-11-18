void EC_GROUP_free(EC_GROUP *group)
{
    if (!group)
        return;

    if (group->meth->group_finish != 0)
        group->meth->group_finish(group);

    EC_pre_comp_free(group);
    BN_MONT_CTX_free(group->mont_data);
    EC_POINT_free(group->generator);
    BN_free(group->order);
    BN_free(group->cofactor);
    OPENSSL_free(group->seed);
    OPENSSL_free(group);
}
