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

static char **
get_environment(char **env_list, size_t env_list_len, int replace)
{
    size_t new_environ_len = env_list_len + 1; /* One extra for last NULL */
    char **new_environ;
    int i;
    int j;
    if (!replace) {
        for (i = 0; environ[i] != NULL; i++) {
            new_environ_len++;
        }
    }

    new_environ = malloc(new_environ_len * sizeof(char *));
    if (new_environ == NULL) {
        err(EXIT_FAILURE, "malloc failed");
    }

    i = 0;
    if (!replace) {
        for (; environ[i] != NULL; i++) {
            if ((asprintf(&new_environ[i], "%s", environ[i])) == -1) {
                err(EXIT_FAILURE, "malloc failed");
            }
        }
    }

    if (env_list == NULL) {
        return;
    }

    for (j = 0; j < env_list_len; j++) {
        if ((asprintf(&new_environ[i++], "%s", env_list[j])) == -1) {
            err(EXIT_FAILURE, "malloc failed");
        }
    }
    new_environ[i] = NULL;
    return new_environ;
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
    environ = get_environment(var_list, nvars, flag_i);
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
    for (i = 0; environ[i] != NULL; i++) {
        free(environ[i]);
    }
    free(environ);
    return exit_status;
}
