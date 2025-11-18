ksba_name_ref (ksba_name_t name)
{
  if (!name)
    fprintf (stderr, "BUG: ksba_name_ref for NULL\n");
  else
    ++name->ref_count;
}
