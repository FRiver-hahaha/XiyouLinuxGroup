#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <pwd.h>
#include <limits.h>

typedef struct {
    char **argv;
    int argc;
} Cmd;

typedef struct {
    Cmd *cmds;
    int ncmd;
    char *in_file;
    char *out_file;
    int out_append;
    int background;
    int valid;
} Pipeline;

static char *prev_dir = NULL;

static void free_cmd(Cmd *c) {
    if (!c) return;
    for (int i = 0; i < c->argc; i++) free(c->argv[i]);
    free(c->argv);
    c->argv = NULL;
    c->argc = 0;
}

static void free_pipeline(Pipeline *p) {
    if (!p) return;
    for (int i = 0; i < p->ncmd; i++) free_cmd(&p->cmds[i]);
    free(p->cmds);
    free(p->in_file);
    free(p->out_file);
    p->cmds = NULL;
    p->ncmd = 0;
    p->in_file = NULL;
    p->out_file = NULL;
}

static void trim_newline(char *s) {
    if (!s) return;
    size_t n = strlen(s);
    while (n && (s[n-1] == '\n' || s[n-1] == '\r')) {
        s[n-1] = 0;
        n--;
    }
}

static int is_special_char(char c) {
    return c=='|' || c=='<' || c=='>' || c=='&';
}

static char **tokenize(const char *line, int *outc) {
    size_t cap = 16;
    int cnt = 0;
    char **tokens = malloc(cap * sizeof(char*));
    const char *p = line;
    while (*p) {
        while (*p && (*p==' ' || *p=='\t')) p++;
        if (!*p) break;
        if (*p=='\'' || *p=='"') {
            char quote = *p++;
            const char *start = p;
            while (*p && *p != quote) p++;
            size_t len = p - start;
            char *tok = malloc(len + 1);
            memcpy(tok, start, len);
            tok[len] = 0;
            if (*p == quote) p++;
            if (cnt >= (int)cap) { cap*=2; tokens = realloc(tokens, cap*sizeof(char*)); }
            tokens[cnt++] = tok;
        } else if (is_special_char(*p)) {
            if (*p == '>') {
                if (*(p+1) == '>') {
                    if (cnt >= (int)cap) { cap*=2; tokens = realloc(tokens, cap*sizeof(char*)); }
                    tokens[cnt++] = strdup(">>");
                    p += 2;
                } else {
                    if (cnt >= (int)cap) { cap*=2; tokens = realloc(tokens, cap*sizeof(char*)); }
                    tokens[cnt++] = strdup(">");
                    p++;
                }
            } else {
                char buf[2] = { *p, 0 };
                if (cnt >= (int)cap) { cap*=2; tokens = realloc(tokens, cap*sizeof(char*)); }
                tokens[cnt++] = strdup(buf);
                p++;
            }
        } else {
            const char *start = p;
            while (*p && !is_special_char(*p) && *p!=' ' && *p!='\t') p++;
            size_t len = p - start;
            char *tok = malloc(len + 1);
            memcpy(tok, start, len);
            tok[len] = 0;
            if (cnt >= (int)cap) { cap*=2; tokens = realloc(tokens, cap*sizeof(char*)); }
            tokens[cnt++] = tok;
        }
    }
    if (outc) *outc = cnt;
    return tokens;
}

static void free_tokens(char **toks, int n) {
    for (int i = 0; i < n; i++) free(toks[i]);
    free(toks);
}

static Pipeline parse_pipeline(char **toks, int n) {
    Pipeline p = {0};
    p.valid = 0;
    if (n == 0) { p.valid = 0; return p; }
    if (n > 0 && strcmp(toks[n-1], "&")==0) {
        p.background = 1;
        n -= 1;
    }
    if (n == 0) { p.valid = 0; return p; }
    int segs = 1;
    for (int i = 0; i < n; i++) if (strcmp(toks[i],"|")==0) segs++;
    p.cmds = calloc(segs, sizeof(Cmd));
    p.ncmd = segs;
    int seg_idx = 0;
    size_t cap = 8;
    p.cmds[0].argv = malloc(cap * sizeof(char*));
    p.cmds[0].argc = 0;
    for (int i = 0; i < n; i++) {
        if (strcmp(toks[i],"|")==0) {
            p.cmds[seg_idx].argv = realloc(p.cmds[seg_idx].argv, (p.cmds[seg_idx].argc+1)*sizeof(char*));
            p.cmds[seg_idx].argv[p.cmds[seg_idx].argc] = NULL;
            seg_idx++;
            cap = 8;
            p.cmds[seg_idx].argv = malloc(cap*sizeof(char*));
            p.cmds[seg_idx].argc = 0;
        } else {
            if (p.cmds[seg_idx].argc >= (int)cap) { cap*=2; p.cmds[seg_idx].argv = realloc(p.cmds[seg_idx].argv, cap*sizeof(char*)); }
            p.cmds[seg_idx].argv[p.cmds[seg_idx].argc++] = strdup(toks[i]);
        }
    }
    p.cmds[seg_idx].argv = realloc(p.cmds[seg_idx].argv, (p.cmds[seg_idx].argc+1)*sizeof(char*));
    p.cmds[seg_idx].argv[p.cmds[seg_idx].argc] = NULL;
    for (int i = 0; i < p.ncmd; i++) if (p.cmds[i].argc==0) { p.valid = 0; return p; }
    for (int i = 0; i < p.cmds[0].argc; i++) {
        if (strcmp(p.cmds[0].argv[i], "<")==0) {
            if (i+1 >= p.cmds[0].argc) { p.valid = 0; return p; }
            free(p.in_file);
            p.in_file = strdup(p.cmds[0].argv[i+1]);
            free(p.cmds[0].argv[i]);
            free(p.cmds[0].argv[i+1]);
            for (int j = i; j+2 <= p.cmds[0].argc; j++) p.cmds[0].argv[j] = p.cmds[0].argv[j+2];
            p.cmds[0].argc -= 2;
            p.cmds[0].argv[p.cmds[0].argc] = NULL;
            i--;
        } else if (strcmp(p.cmds[0].argv[i], ">")==0 || strcmp(p.cmds[0].argv[i], ">>")==0) {
            p.valid = 0;
            return p;
        }
    }
    for (int i = 0; i < p.cmds[p.ncmd-1].argc; i++) {
        if (strcmp(p.cmds[p.ncmd-1].argv[i], ">")==0 || strcmp(p.cmds[p.ncmd-1].argv[i], ">>")==0) {
            if (i+1 >= p.cmds[p.ncmd-1].argc) { p.valid = 0; return p; }
            p.out_append = strcmp(p.cmds[p.ncmd-1].argv[i], ">>")==0;
            free(p.out_file);
            p.out_file = strdup(p.cmds[p.ncmd-1].argv[i+1]);
            free(p.cmds[p.ncmd-1].argv[i]);
            free(p.cmds[p.ncmd-1].argv[i+1]);
            for (int j = i; j+2 <= p.cmds[p.ncmd-1].argc; j++) p.cmds[p.ncmd-1].argv[j] = p.cmds[p.ncmd-1].argv[j+2];
            p.cmds[p.ncmd-1].argc -= 2;
            p.cmds[p.ncmd-1].argv[p.cmds[p.ncmd-1].argc] = NULL;
            i--;
        } else if (strcmp(p.cmds[p.ncmd-1].argv[i], "<")==0) {
            p.valid = 0;
            return p;
        }
    }
    for (int s = 1; s < p.ncmd-1; s++) {
        for (int i = 0; i < p.cmds[s].argc; i++) {
            if (strcmp(p.cmds[s].argv[i], "<")==0 || strcmp(p.cmds[s].argv[i], ">")==0 || strcmp(p.cmds[s].argv[i], ">>")==0) {
                p.valid = 0;
                return p;
            }
        }
    }
    p.valid = 1;
    return p;
}

static void print_prompt() {
    char host[256] = {0};
    gethostname(host, sizeof(host)-1);
    const char *user = getenv("USER");
    if (!user) {
        struct passwd *pw = getpwuid(getuid());
        if (pw && pw->pw_name) user = pw->pw_name;
        else user = "user";
    }
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) strcpy(cwd, "?");
    const char *home = getenv("HOME");
    char shown[PATH_MAX];
    if (home && strncmp(cwd, home, strlen(home))==0) {
        snprintf(shown, sizeof(shown), "~%s", cwd+strlen(home));
    } else {
        snprintf(shown, sizeof(shown), "%s", cwd);
    }
    printf("%s@%s %s $ ", user, host[0]?host:"host", shown);
    fflush(stdout);
}

static int builtin_cd(Cmd *c) {
    if (c->argc < 2) {
        const char *home = getenv("HOME");
        if (!home) return -1;
        char *cur = getcwd(NULL, 0);
        int r = chdir(home);
        if (r==0) {
            free(prev_dir);
            prev_dir = cur;
        } else {
            free(cur);
        }
        return r;
    }
    if (strcmp(c->argv[1], "-")==0) {
        if (!prev_dir) return -1;
        char *cur = getcwd(NULL, 0);
        int r = chdir(prev_dir);
        if (r==0) {
            free(prev_dir);
            prev_dir = cur;
            printf("%s\n", prev_dir);
        } else {
            free(cur);
        }
        return r;
    } else {
        char *cur = getcwd(NULL, 0);
        int r = chdir(c->argv[1]);
        if (r==0) {
            free(prev_dir);
            prev_dir = cur;
        } else {
            free(cur);
        }
        return r;
    }
}

static int is_builtin(Cmd *c) {
    if (c->argc==0) return 0;
    if (strcmp(c->argv[0], "cd")==0) return 1;
    if (strcmp(c->argv[0], "exit")==0) return 2;
    return 0;
}

static void restore_default_signals() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_DFL;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
}

static void ignore_shell_signals() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
}

static void reap_children() {
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0) {}
}

static int run_pipeline(Pipeline *p) {
    if (!p->valid) {
        fprintf(stderr, "syntax error\n");
        return 1;
    }
    if (p->ncmd==1) {
        int b = is_builtin(&p->cmds[0]);
        if (b==1) {
            if (p->in_file || p->out_file) {
                fprintf(stderr, "redirection not supported for builtin\n");
                return 1;
            }
            int r = builtin_cd(&p->cmds[0]);
            if (r!=0) perror("cd");
            return r!=0;
        } else if (b==2) {
            exit(0);
        }
    }
    int n = p->ncmd;
    int (*pipes)[2] = NULL;
    if (n>1) {
        pipes = calloc(n-1, sizeof(int[2]));
        for (int i = 0; i < n-1; i++) {
            if (pipe(pipes[i]) < 0) {
                perror("pipe");
                for (int k=0;k<i;k++){close(pipes[k][0]);close(pipes[k][1]);}
                free(pipes);
                return 1;
            }
        }
    }
    pid_t *pids = calloc(n, sizeof(pid_t));
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            for (int k=0;k<n-1;k++){ if (pipes){close(pipes[k][0]);close(pipes[k][1]);} }
            free(pipes);
            free(pids);
            return 1;
        }
        if (pid == 0) {
            restore_default_signals();
            if (i==0 && p->in_file) {
                int fd = open(p->in_file, O_RDONLY);
                if (fd < 0) { perror("open"); _exit(1); }
                if (dup2(fd, STDIN_FILENO) < 0) { perror("dup2"); _exit(1); }
                close(fd);
            }
            if (i>0) {
                if (dup2(pipes[i-1][0], STDIN_FILENO) < 0) { perror("dup2"); _exit(1); }
            }
            if (i < n-1) {
                if (dup2(pipes[i][1], STDOUT_FILENO) < 0) { perror("dup2"); _exit(1); }
            } else {
                if (p->out_file) {
                    int flags = O_WRONLY | O_CREAT | (p->out_append ? O_APPEND : O_TRUNC);
                    int fd = open(p->out_file, flags, 0666);
                    if (fd < 0) { perror("open"); _exit(1); }
                    if (dup2(fd, STDOUT_FILENO) < 0) { perror("dup2"); _exit(1); }
                    close(fd);
                }
            }
            if (pipes) {
                for (int k = 0; k < n-1; k++) {
                    close(pipes[k][0]);
                    close(pipes[k][1]);
                }
            }
            int b = is_builtin(&p->cmds[i]);
            if (b==1) {
                int r = builtin_cd(&p->cmds[i]);
                if (r!=0) perror("cd");
                _exit(r!=0);
            } else if (b==2) {
                _exit(0);
            }
            execvp(p->cmds[i].argv[0], p->cmds[i].argv);
            perror("execvp");
            _exit(127);
        } else {
            pids[i] = pid;
        }
    }
    if (pipes) {
        for (int k = 0; k < n-1; k++) {
            close(pipes[k][0]);
            close(pipes[k][1]);
        }
    }
    free(pipes);
    int status = 0;
    if (p->background) {
        printf("[bg] %d\n", pids[n-1]);
    } else {
        for (int i = 0; i < n; i++) {
            int st;
            if (waitpid(pids[i], &st, 0) > 0) status = st;
        }
    }
    free(pids);
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

int main() {
    ignore_shell_signals();
    char *line = NULL;
    size_t cap = 0;
    prev_dir = getcwd(NULL, 0);
    while (1) {
        reap_children();
        print_prompt();
        ssize_t m = getline(&line, &cap, stdin);
        if (m < 0) break;
        trim_newline(line);
        if (line[0]==0) continue;
        int nt = 0;
        char **toks = tokenize(line, &nt);
        if (nt==0) { free_tokens(toks, nt); continue; }
        Pipeline p = parse_pipeline(toks, nt);
        free_tokens(toks, nt);
        if (!p.valid) {
            fprintf(stderr, "invalid command\n");
            free_pipeline(&p);
            continue;
        }
        if (p.ncmd==1) {
            int b = is_builtin(&p.cmds[0]);
            if (b==1) {
                int r = builtin_cd(&p.cmds[0]);
                if (r!=0) perror("cd");
                free_pipeline(&p);
                continue;
            } else if (b==2) {
                free_pipeline(&p);
                break;
            }
        }
        run_pipeline(&p);
        free_pipeline(&p);
    }
    free(prev_dir);
    free(line);
    return 0;
}

