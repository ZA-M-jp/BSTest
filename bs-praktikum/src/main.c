
/*
 * main.c — Server-Einstiegspunkt
 * ================================
 * AUFGABENVERTEILUNG:
 *
 *   Ehsan implementiert:
 *     - Den fork()-Block im Accept-Loop
 *     - Den SIGCHLD-Handler (Zombie-Prozesse vermeiden)
 *     - Den SIGINT-Handler (Ctrl+C -> store_cleanup)
 *
 *   Muhannad implementiert:
 *     - socket(), setsockopt(), bind(), listen() vor dem Loop
 *     - handle_client()  — laeuft im Kindprozess
 *     - process_command() — parst PUT/GET/DEL/SUB und antwortet
 *
 * Ablauf wenn ein Client sich verbindet:
 *
 *   Elternprozess:
 *     accept() -> client_fd
 *     fork()
 *     -> Kindprozess:  close(server_fd), handle_client(client_fd), exit(0)
 *     -> Elternprozess: close(client_fd), weiter warten
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../include/kvstore.h"
#include "../include/pubsub.h"

/* ════════════════════════════════════════════════════════════════════
 * EHSAN — Signal-Handler
 * ════════════════════════════════════════════════════════════════════ */
static void handle_sigint(int sig) {
 (void)sig;
 store_cleanup();
 exit(0);
}

static void handle_sigchld(int sig) {
 (void)sig;

 while (waitpid(-1, NULL, WNOHANG) > 0) {
 }
}
/* TODO: Ehsan
 *
 * SIGINT-Handler (Ctrl+C):
 *   static void handle_sigint(int sig) {
 *       (void)sig;
 *       store_cleanup();
 *       exit(0);
 *   }
 *
 * SIGCHLD-Handler (beendete Kindprozesse aufraumen):
 *   static void handle_sigchld(int sig) {
 *       (void)sig;
 *       while (waitpid(-1, NULL, WNOHANG) > 0);
 *   }
 *
 */

static void process_command(int client_fd, char *line) {
 char *cmd = strtok(line, " ");

 if (cmd == NULL) {
  return;
 }

 if (strcmp(cmd, "PUT") == 0) {
  char *key = strtok(NULL, " ");
  char *value = strtok(NULL, "");

  if (key && value) {
   store_put(key, value);

   char response[1024];
   snprintf(response, sizeof(response), "PUT:%s:%s\n", key, value);
   send(client_fd, response, strlen(response), 0);
  }
 }

 else if (strcmp(cmd, "GET") == 0) {
  char *key = strtok(NULL, " ");

  if (key) {
   char value[256];
   char response[1024];

   if (store_get(key, value) == 0) {
    snprintf(response, sizeof(response),
             "GET:%s:%s\n",
             key, value);
   } else {
    snprintf(response, sizeof(response),
             "GET:%s:key_nonexistent\n",
             key);
   }

   send(client_fd, response, strlen(response), 0);
  }
 }
 else if (strcmp(cmd, "DEL") == 0) {
  char *key = strtok(NULL, " ");

  if (key) {
   store_del(key);

   char response[1024];
   snprintf(response, sizeof(response),
            "DEL:%s:key_deleted\n",
            key);

   send(client_fd, response, strlen(response), 0);
  }
 }
}




static void handle_client(int client_fd) {

 char buf[1024];
 int n;

 printf("[server] client connected\n");

 while ((n = recv(client_fd, buf, sizeof(buf) - 1, 0)) > 0) {

  buf[n] = '\0';


  buf[strcspn(buf, "\r\n")] = '\0';

  process_command(client_fd, buf);
 }

 close(client_fd);
}

int main(void) {
    printf("[server] starting...\n");

    signal(SIGINT, handle_sigint);
    signal(SIGCHLD, handle_sigchld);

    store_init();
 int server_fd;
 int opt = 1;
 struct sockaddr_in server_addr;
/* ════════════════════════════════════════════════════════════════════
 * MUHANNAD — Client-Handler
 * ════════════════════════════════════════════════════════════════════ */
 server_fd = socket(AF_INET, SOCK_STREAM, 0);

 if (server_fd < 0) {
  perror("socket");
  exit(1);
 }

 if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
  perror("setsockopt");
  exit(1);
 }


 server_addr.sin_family = AF_INET;
 server_addr.sin_addr.s_addr = INADDR_ANY;
 server_addr.sin_port = htons(5555);

 if (bind(server_fd, (struct sockaddr *)&server_addr,
         sizeof(server_addr)) < 0) {
  perror("bind");
  exit(1);
         }

 if (listen(server_fd, 5) < 0) {
  perror("listen");
  exit(1);
 }

 printf("[server] listening on port 8080...\n");
 while (1) {
  int client_fd;

  client_fd = accept(server_fd, NULL, NULL);

  if (client_fd < 0) {
   perror("accept");
   continue;
  }

  pid_t pid = fork();

  if (pid == 0) {
   close(server_fd);
   handle_client(client_fd);
   exit(0);
  } else {
   close(client_fd);
  }
 }
 close(server_fd);
 store_cleanup();
 printf("[server] stopped.\n");
 return 0;
}