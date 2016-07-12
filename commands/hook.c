#include <string.h>
#include <stdlib.h>
#include <groonga/plugin.h>

#ifdef __GNUC__
# define GNUC_UNUSED __attribute__((__unused__))
#else
# define GNUC_UNUSED
#endif

typedef struct {
  grn_id target;
  unsigned int section;
} default_set_value_hook_data;

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
  grn_bool no_target_only = GRN_TRUE;

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
  var = grn_plugin_proc_get_var(ctx, user_data, "no_target_only", -1);
  if (GRN_TEXT_LEN(var) != 0) {
    no_target_only = atoi(GRN_TEXT_VALUE(var));
  }
  table = grn_ctx_get(ctx, table_name, table_len);
  column = grn_obj_column(ctx, table, column_name, column_len);
  if (!column) {
    GRN_PLUGIN_ERROR(ctx, GRN_INVALID_ARGUMENT,
                     "[hook_delete] missing column");
    return NULL;
  }

  nhooks = grn_obj_get_nhooks(ctx, column, GRN_HOOK_SET);

  if (nhooks > 0) {
    if (!no_target_only) {
      grn_obj_delete_hook(ctx, column, GRN_HOOK_SET, 0);
    } else {
      grn_obj *hook;
      unsigned int i;
      grn_obj buf;
      grn_obj data;
      GRN_TEXT_INIT(&buf, 0);
      GRN_TEXT_INIT(&data, 0);
      for (i = 0; i < nhooks; i++) {
        default_set_value_hook_data *hdata;
        grn_obj *target;
        GRN_BULK_REWIND(&buf);
        GRN_BULK_REWIND(&data);
        hook = grn_obj_get_hook(ctx, column, GRN_HOOK_SET, i, &data);
        hdata = (void *)GRN_TEXT_VALUE(&data);
        target = grn_ctx_at(ctx, hdata->target);
        grn_inspect_name(ctx, &buf, target);
        if (GRN_TEXT_LEN(&buf) > 5 &&
            !memcmp(GRN_TEXT_VALUE(&buf), "(nil)", 5)) {
          grn_obj_delete_hook(ctx, column, GRN_HOOK_SET, i);
        }

        GRN_BULK_REWIND(&buf);
        grn_inspect_name(ctx, &buf, hook);
      }
      GRN_OBJ_FIN(ctx, &buf);
      GRN_OBJ_FIN(ctx, &data);

    }
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
  unsigned int i;

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
  if (!column) {
    GRN_PLUGIN_ERROR(ctx, GRN_INVALID_ARGUMENT,
                     "[hook_status] missing column");
    return NULL;
  }

  nhooks = grn_obj_get_nhooks(ctx, column, GRN_HOOK_SET);

  grn_ctx_output_array_open(ctx, "HOOK", 2);
  grn_ctx_output_int32(ctx, nhooks);
 
  grn_ctx_output_array_open(ctx, "HOOKS", nhooks);

  if (nhooks) {
    grn_obj *hook;
    unsigned int i;

    grn_obj buf;
    grn_obj data;
    GRN_TEXT_INIT(&buf, 0);
    GRN_TEXT_INIT(&data, 0);
    for (i = 0; i < nhooks; i++) {
      default_set_value_hook_data *hdata;
      grn_obj *target;
      GRN_BULK_REWIND(&buf);
      GRN_BULK_REWIND(&data);
      hook = grn_obj_get_hook(ctx, column, GRN_HOOK_SET, i, &data);
      hdata = (void *)GRN_TEXT_VALUE(&data);
      target = grn_ctx_at(ctx, hdata->target);

      grn_ctx_output_array_open(ctx, "DATA", 2);
      grn_inspect_name(ctx, &buf, target);
      grn_ctx_output_str(ctx, GRN_TEXT_VALUE(&buf), GRN_TEXT_LEN(&buf));
      GRN_BULK_REWIND(&buf);
      grn_inspect_name(ctx, &buf, hook);
      grn_ctx_output_str(ctx, GRN_TEXT_VALUE(&buf), GRN_TEXT_LEN(&buf));
      grn_ctx_output_array_close(ctx);
    }
    GRN_OBJ_FIN(ctx, &buf);
    GRN_OBJ_FIN(ctx, &data);
  }
  grn_ctx_output_array_close(ctx);
  grn_ctx_output_array_close(ctx);

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
  grn_expr_var vars[3];

  grn_plugin_expr_var_init(ctx, &vars[0], "table", -1);
  grn_plugin_expr_var_init(ctx, &vars[1], "column", -1);
  grn_plugin_expr_var_init(ctx, &vars[2], "no_target_only", -1);
  grn_plugin_command_create(ctx, "hook_delete", -1, command_hook_delete, 3, vars);

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
