bool AppCacheGroup::FindObserver(
    const UpdateObserver* find_me,
    const base::ObserverList<UpdateObserver>::Unchecked& observer_list) {
  return observer_list.HasObserver(find_me);
}
