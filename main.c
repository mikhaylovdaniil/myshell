#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

/* writes formated logline about entered cmd into stream */
void log_cmd(FILE *stream, char const *str_time, char *buff);

void log_errinp(FILE *stream, char const *str_time);

/* replaces \n character with \0 */
void rm_newlc(char *line);

int char_count(char const *str, char const c);
char **split_cmdline(char const *cmdLine, int *argSize);

int main(void)
{
    /* handling signals */
    
    sigset_t blocked_set, blocked_oldset;
    sigemptyset(&blocked_set);
    sigaddset(&blocked_set, SIGINT);
    
    if (sigprocmask(SIG_BLOCK, &blocked_set, &blocked_oldset) == -1) {
        perror("sigprocmask SIG_BLOCK");
        return 1;
    }

    /* Logging data */
    time_t currentTime;
    char *str_time = NULL;
    FILE *logStream = fopen("./logs.txt", "a");

    /* Input data */
    char *cmdLine = NULL;
    size_t size = 0;
    ssize_t nread;

    char* execBuff = malloc(256);
    char **cmdArgs = NULL;
    int ca_size = 0;

    int isRunning = 1;
    while(isRunning) {
        time(&currentTime);
        str_time = ctime(&currentTime);         
        rm_newlc(str_time); /* Deleting new line symbol */

        printf("[%s] Enter the command: ", str_time);
        nread = getline(&cmdLine, &size, stdin);

        if(cmdLine[nread-1] == EOF || nread == -1) {
            isRunning = 0;
        }

        if(strstr(cmdLine, "exit") != NULL) {
            isRunning = 0;
        }

        pid_t cmdPid = fork();

        if(cmdPid == 0) {
            cmdArgs = split_cmdline(cmdLine, &ca_size);
            sprintf(execBuff, "./commands/%s", cmdArgs[0]);
            execv(execBuff, cmdArgs);
            
            /* if in first path didn't work */
            sprintf(execBuff, "/usr/bin/%s", cmdArgs[0]);
            execv(execBuff, cmdArgs);
            exit(3);
        } else {
            waitpid(cmdPid, NULL, 0);
        }

        /* logging */
        log_cmd(logStream, str_time, cmdLine);
    }

    fclose(logStream);
    free(cmdArgs);
    return 0;
}

void log_cmd(FILE *stream, char const *str_time, char *buff)
{
    fprintf(stream, "[%s] [INFO] User entered %s command.\n", str_time, buff);
    fflush(stream);
}

void log_errinp(FILE *stream, char const *str_time)
{
    fprintf(stream, "[%s] [MISTAKE] User mismatched input.\n", str_time);
    fflush(stream);
}

void rm_newlc(char *line) 
{
    while(line) {
        if(*line == '\n') {
            *line = '\0';
            return;
        }
        ++line;
    }
}

char **split_cmdline(char const *cmdLine, int *argSize)
{
    char *buf = malloc(sizeof(*cmdLine) * strlen(cmdLine) + 1);
    strcpy(buf, cmdLine);

    char *token;
    const char s[2] = " ";
 
    int wordCount = char_count(cmdLine, ' ') + 1;
    char **argBuf = malloc(sizeof(char *) * wordCount + 1);
    
    token = strtok(buf, s);
    int i = 0;
    while(token != NULL && i < wordCount) {
      argBuf[i] = malloc(sizeof(*token) * strlen(token) + 1);
      strcpy(argBuf[i], token);
    
      token = strtok(NULL, s);
      i++;
    }
    
    argBuf[wordCount] = NULL;
    
    *argSize = wordCount;

    free(buf);
    return argBuf;
}

int char_count(char const *str, char const c)
{
    int count = 0;
    while(*str != '\0' && *str != '\n') {
        if(*str == c) count++;
        str++;
    }
    return count;
}
