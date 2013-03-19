#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _GNU_SOURCE /* Required for asprintf on GNU platform */
extern char **environ;

static void
print_env(void)
{
    int i;
    for (i = 0; environ[i] != NULL; i++) {
        printf("%s\n", environ[i]);
    }
}

static void
set_environment(char **env_list, size_t env_list_len, int replace)
{
    char *clearenv[1] = {NULL};
    int i;
    if (replace) {
        environ = clearenv;
    }

    for (i = 0; i < env_list_len; i++) {
        putenv(env_list[i]);
    }
}

static int
run_cmd(const char *cmd)
{
    return system(cmd);
}

int
main(int argc, char **argv)
{
    int i;
    int j;
    int flag_i = 0; /* -i specified or not */
    char **var_list = NULL; /* The environment variable list */
    char *cmd = NULL;
    char *cmd_args;
    size_t cmd_args_len;
    int env_var_pos_list[argc];
    size_t nvars = 0;
    size_t equals_pos;
    int cmd_args_index = 0;
    int exit_status;
    
    if (argc == 1) {
        print_env();
        return 0;
    }

    memset((void *) env_var_pos_list, 0, argc * sizeof(int));

    for (i = 1; argv[i] != NULL; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            flag_i++;
            continue;
        }

        if (strchr(argv[i], '=') != NULL) {
            env_var_pos_list[i] = 1;
            nvars++;
            continue;
        }

        if (cmd == NULL) {
            cmd = argv[i];
            continue;
        }

        if (cmd != NULL) {
            cmd_args_index = i;
            break;
        }
    }

    if (nvars) {
        var_list = malloc(nvars * sizeof(char *));
        if (var_list == NULL) {
            err(EXIT_FAILURE, "malloc failed");
        }

        for (i = 1, j = 0; i < argc; i++) {
            if (env_var_pos_list[i] == 0) {
                continue;
            }
            
            var_list[j++] = argv[i];
        }
    }
    set_environment(var_list, nvars, flag_i);
    if (!cmd) {
        print_env();
        return 0;
    }
    free(var_list);

    cmd_args_len = strlen(cmd);
    for (i = cmd_args_index; i && argv[i] != NULL; i++) {
        cmd_args_len++; /* For space between two tokens */
        cmd_args_len += strlen(argv[i]);
    }
    cmd_args_len++; /* NUL byte */

    if ((cmd_args = malloc(cmd_args_len)) == NULL)  {
        err(EXIT_FAILURE, "malloc failed");
    }
    strncpy(cmd_args, cmd, strlen(cmd) + 1);

    for (i = cmd_args_index; i && argv[i] != NULL; i++) {
        strncat(cmd_args, " ", 1);
        strncat(cmd_args, argv[i], strlen(argv[i]) + 1);
    }
    exit_status = run_cmd(cmd_args);
    free(cmd_args);
    return exit_status;
}
