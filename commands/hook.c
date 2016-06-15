#include <string.h>
#include <stdlib.h>
#include <groonga/plugin.h>

#ifdef __GNUC__
# define GNUC_UNUSED __attribute__((__unused__))
#else
# define GNUC_UNUSED
#endif

static grn_obj *
command_hook_delete(grn_ctx *ctx, GNUC_UNUSED int nargs, GNUC_UNUSED grn_obj **args,
                    grn_user_data *user_data)
{
  grn_obj *var, *table, *column;
  unsigned int nhooks = 0;

  char *table_name = NULL;
  unsigned int table_len = 0;
  char *column_name = NULL;
  unsigned int column_len = 0;

  var = grn_plugin_proc_get_var(ctx, user_data, "table", -1);
  if (GRN_TEXT_LEN(var) != 0) {
    table_name = GRN_TEXT_VALUE(var);
    table_len = GRN_TEXT_LEN(var);
  }
  var = grn_plugin_proc_get_var(ctx, user_data, "column", -1);
  if (GRN_TEXT_LEN(var) != 0) {
    column_name = GRN_TEXT_VALUE(var);
    column_len = GRN_TEXT_LEN(var);
  }

  table = grn_ctx_get(ctx, table_name, table_len);
  column = grn_obj_column(ctx, table, column_name, column_len);

  nhooks = grn_obj_get_nhooks(ctx, column, GRN_HOOK_SET);

  if (nhooks > 0) {
    grn_obj_delete_hook(ctx, column, GRN_HOOK_SET, 0);
  }

  nhooks = grn_obj_get_nhooks(ctx, column, GRN_HOOK_SET);
  grn_ctx_output_int32(ctx, nhooks);

  return NULL;
}

static grn_obj *
command_hook_status(grn_ctx *ctx, GNUC_UNUSED int nargs, GNUC_UNUSED grn_obj **args,
                    grn_user_data *user_data)
{
  grn_obj *var, *table, *column;
  unsigned int nhooks = 0;

  char *table_name = NULL;
  unsigned int table_len = 0;
  char *column_name = NULL;
  unsigned int column_len = 0;

  var = grn_plugin_proc_get_var(ctx, user_data, "table", -1);
  if (GRN_TEXT_LEN(var) != 0) {
    table_name = GRN_TEXT_VALUE(var);
    table_len = GRN_TEXT_LEN(var);
  }
  var = grn_plugin_proc_get_var(ctx, user_data, "column", -1);
  if (GRN_TEXT_LEN(var) != 0) {
    column_name = GRN_TEXT_VALUE(var);
    column_len = GRN_TEXT_LEN(var);
  }

  table = grn_ctx_get(ctx, table_name, table_len);
  column = grn_obj_column(ctx, table, column_name, column_len);

  nhooks = grn_obj_get_nhooks(ctx, column, GRN_HOOK_SET);
  grn_ctx_output_int32(ctx, nhooks);

  return NULL;
}

grn_rc
GRN_PLUGIN_INIT(GNUC_UNUSED grn_ctx *ctx)
{
  return GRN_SUCCESS;
}

grn_rc
GRN_PLUGIN_REGISTER(grn_ctx *ctx)
{
  grn_expr_var vars[2];

  grn_plugin_expr_var_init(ctx, &vars[0], "table", -1);
  grn_plugin_expr_var_init(ctx, &vars[1], "column", -1);
  grn_plugin_command_create(ctx, "hook_delete", -1, command_hook_delete, 2, vars);

  grn_plugin_expr_var_init(ctx, &vars[0], "table", -1);
  grn_plugin_expr_var_init(ctx, &vars[1], "column", -1);
  grn_plugin_command_create(ctx, "hook_status", -1, command_hook_status, 2, vars);

  return ctx->rc;
}

grn_rc
GRN_PLUGIN_FIN(GNUC_UNUSED grn_ctx *ctx)
{
  return GRN_SUCCESS;
}
