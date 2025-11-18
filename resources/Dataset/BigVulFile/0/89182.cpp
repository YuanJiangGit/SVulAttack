extern void onig_add_end_call(void (*func)(void))
{
  EndCallListItemType* item;

  item = (EndCallListItemType* )xmalloc(sizeof(*item));
  if (item == 0) return ;

  item->next = EndCallTop;
  item->func = func;

  EndCallTop = item;
}
