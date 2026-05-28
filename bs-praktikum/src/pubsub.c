/*
 * pubsub.c — Publish/Subscribe Mechanismus
 * =========================================
 * ZAHRA implementiert alle drei Funktionen.
 *
 * Grundprinzip (Polling):
 *   - Jeder Kindprozess hat eine lokale subs_t-Struktur auf dem Stack
 *   - Er speichert beim SUB-Befehl die aktuelle version des Keys
 *   - In der Empfangs-Schleife prueft er regelmaessig ob version gestiegen ist
 *   - Wenn ja: schickt er das last_event an den Client via write()
 *
 * Warum kein gemeinsamer Speicher fuer Subscriptions?
 *   Weil jeder Kindprozess nur seine eigenen Clients kennen muss.
 *   Die Daten im Store (entry_t) sind gemeinsam — die Abo-Liste nicht.
 */

#include "../include/pubsub.h"
#include <string.h>
#include <unistd.h>

void subs_init(subs_t *s) {
    memset(s, 0, sizeof(subs_t));
}

int subs_add(subs_t *s, const char *key) {
    if (s->count >= MAX_SUBS_PER_CLIENT) {
        return -1;                                  /* Liste voll */
    }
    strncpy(s->keys[s->count], key, MAX_KEY_LEN - 1);
    s->keys[s->count][MAX_KEY_LEN - 1] = '\0';      /* sicher terminieren */
    s->last_versions[s->count] = store_get_version(key);  /* Startwert merken */
    s->count++;
    return 0;
}

void subs_check_and_notify(subs_t *s, int client_fd) {
    for (int i = 0; i < s->count; i++) {
        int v = store_get_version(s->keys[i]);
        if (v > s->last_versions[i]) {              /* Version gestiegen? */
            char event[MAX_EVENT_LEN];
            store_get_event(s->keys[i], event);
            write(client_fd, event, strlen(event)); /* an MEINEN Client */
            write(client_fd, "\n", 1);
            s->last_versions[i] = v;                /* als gesehen markieren */
        }
    }
}