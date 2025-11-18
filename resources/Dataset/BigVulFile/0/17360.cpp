set_machine_vars ()
{
  SHELL_VAR *temp_var;

  temp_var = set_if_not ("HOSTTYPE", HOSTTYPE);
  temp_var = set_if_not ("OSTYPE", OSTYPE);
  temp_var = set_if_not ("MACHTYPE", MACHTYPE);

  temp_var = set_if_not ("HOSTNAME", current_host_name);
}
