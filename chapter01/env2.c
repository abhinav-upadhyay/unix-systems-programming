#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char **environ;
typedef struct {
    char *name;
    char *value;
} env_var_t;

static void
print_env(void)
{
    int i;
    for (i = 0; environ[i] != NULL; i++) {
        printf("%s\n", environ[i]);
    }
}

static void
set_environment(env_var_t **env_list, int replace)
{
}

int
main(int argc, char **argv)
{
    int i;
    int j;
    int flag_i = 0; /* -i specified or not */
    env_var_t **var_list; /* The environment variable list */
    char *cmd = NULL;
    int env_var_pos_list[argc];
    size_t nvars = 0;
    size_t equals_pos;
    int cmd_args_index;
    
    if (argc == 1) {
        print_env();
        return 0;
    }

    memset((void *) env_var_pos_list, 0, argc);

    for (i = 0; argv[i] != NULL; i++) {
        if (strcmp(argv[i], "-i")) {
            flag_i++;
            continue;
        }

        if ((equals_pos = strchr(argv[i], '=')) != NULL) {
            env_var_pos_list[i] = equals_pos;
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
        var_list = calloc(nvars, sizeof(env_var_t));
        if (var_list == NULL) {
            err(EXIT_FAILURE, "malloc failed");
        }

        for (i = 0, j = 0; i < argc; i++) {
            if (env_var_pos_list[i] == 0) {
                continue;
            }
            equals_pos = env_var_pos_list[i];
            argv[i][equals_pos] = 0;
            
            var_list[j]->name = argv[i];
            var_list[j++]->value = argv[i] + equals_pos + 1;
        }
    }


        
            
    return 0;
}
