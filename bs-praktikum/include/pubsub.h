#ifndef PUBSUB_H
#define PUBSUB_H

/*
 * pubsub.h — Publish/Subscribe
 * ==============================
 * Zahra implementiert alle Funktionen in pubsub.c.
 * Mohannad ruft subs_init() und subs_check_and_notify() in
 * seiner handle_client()-Schleife auf.
 *
 * Wie Pub/Sub funktioniert:
 *   1. Client schickt: SUB foo
 *   2. Mohannad ruft subs_add(&subs, "foo") auf
 *   3. In der Empfangs-Schleife ruft Mohannad regelmaessig
 *      subs_check_and_notify(&subs, client_fd) auf
 *   4. Zahra vergleicht die aktuelle version im Store mit der
 *      gespeicherten — hat sie sich geaendert, wird ein write()
 *      an client_fd geschickt
 */

#include "kvstore.h"

/* ── Limits ───────────────────────────────────────────────────────── */
#define MAX_SUBS_PER_CLIENT 16

/*
 * subs_t: Abo-Liste eines einzelnen Clients.
 * NICHT im Shared Memory — jeder Kindprozess hat seine eigene Kopie
 * auf dem Stack. Das ist absichtlich so.
 *
 *   keys[]          - abonnierte Key-Namen
 *   last_versions[] - version-Stand zum Zeitpunkt des SUB-Befehls
 *   count           - wie viele aktive Abos
 */
typedef struct {
    char keys[MAX_SUBS_PER_CLIENT][MAX_KEY_LEN];
    int  last_versions[MAX_SUBS_PER_CLIENT];
    int  count;
} subs_t;

/* ── API ──────────────────────────────────────────────────────────── */
/* Zahra implementiert alles in pubsub.c                              */

/* Nullt die subs_t-Struktur. Einmal am Anfang von handle_client aufrufen. */
void subs_init(subs_t *s);

/* Fuegt key zur Abo-Liste hinzu. Speichert aktuelle version als Startwert.
 * Rueckgabe: 0 = OK, -1 = Liste voll oder Key existiert nicht.      */
int subs_add(subs_t *s, const char *key);

/* Prueft alle abonnierten Keys. Wenn version gestiegen ist:
 * schreibt das last_event via write(client_fd, ...).
 * Diese Funktion in der handle_client-Schleife regelmaessig aufrufen.*/
void subs_check_and_notify(subs_t *s, int client_fd);

#endif /* PUBSUB_H */
