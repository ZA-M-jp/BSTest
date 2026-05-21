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
    /* TODO: Zahra
     * Einfach: memset(s, 0, sizeof(subs_t));
     * Nullt count und alle last_versions auf 0.
     */
    (void)s;
}

int subs_add(subs_t *s, const char *key) {
    /* TODO: Zahra
     *
     * 1. Pruefen ob s->count < MAX_SUBS_PER_CLIENT
     * 2. key in s->keys[s->count] kopieren
     * 3. WICHTIG: aktuelle version als Startwert speichern:
     *    s->last_versions[s->count] = store_get_version(key);
     *    (Sonst werden alte Events nochmal geschickt!)
     * 4. s->count++
     * return 0 bei Erfolg, -1 wenn Liste voll
     */
    (void)s;
    (void)key;
    return 0;
}

void subs_check_and_notify(subs_t *s, int client_fd) {
    /* TODO: Zahra
     *
     * for (int i = 0; i < s->count; i++) {
     *     int v = store_get_version(s->keys[i]);
     *     if (v > s->last_versions[i]) {
     *         char event[MAX_EVENT_LEN];
     *         store_get_event(s->keys[i], event);
     *         write(client_fd, event, strlen(event));
     *         write(client_fd, "\n", 1);
     *         s->last_versions[i] = v;
     *     }
     * }
     *
     * Hinweis: write() kann fehlschlagen wenn der Socket weg ist.
     * Einfach ignorieren — der naechste recv() bemerkt es.
     */
    (void)s;
    (void)client_fd;
}
