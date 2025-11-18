void init_all_transactions()
{
    UINT8 txn_indx=0;
 for(txn_indx=0; txn_indx < MAX_TRANSACTIONS_PER_SESSION; txn_indx++)
 {
        initialize_transaction(txn_indx);
 }
}
