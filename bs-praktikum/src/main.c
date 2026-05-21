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
 *   Mohannad implementiert:
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
#include "../include/kvstore.h"
#include "../include/pubsub.h"

/* ════════════════════════════════════════════════════════════════════
 * EHSAN — Signal-Handler
 * ════════════════════════════════════════════════════════════════════ */

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
 * In main() registrieren:
 *   signal(SIGINT,  handle_sigint);
 *   signal(SIGCHLD, handle_sigchld);
 */

/* ════════════════════════════════════════════════════════════════════
 * MOHANNAD — Client-Handler
 * ════════════════════════════════════════════════════════════════════ */

/* TODO: Mohannad
 *
 * void handle_client(int client_fd) {
 *     subs_t subs;
 *     subs_init(&subs);
 *
 *     char buf[1024];
 *     int n;
 *     while ((n = recv(client_fd, buf, sizeof(buf)-1, 0)) > 0) {
 *         buf[n] = '\0';
 *         process_command(client_fd, buf, &subs);
 *         subs_check_and_notify(&subs, client_fd);  <- Zahra
 *     }
 *     close(client_fd);
 * }
 *
 * void process_command(int fd, char *line, subs_t *subs) {
 *     // \r\n entfernen
 *     // strtok fuer PUT/GET/DEL/SUB
 *     // store_put / store_get / store_del aufrufen
 *     // Antwort mit write(fd, ...) zurueckschicken
 * }
 */

int main(void) {
    printf("[server] starting...\n");

    /* Ehsan: Signal-Handler registrieren */
    /* Ehsan: store_init() aufrufen       */
    store_init();

    /* Mohannad: socket(), bind(), listen() */
    /* Ehsan:    accept-loop mit fork()     */

    store_cleanup();
    printf("[server] stopped.\n");
    return 0;
}
