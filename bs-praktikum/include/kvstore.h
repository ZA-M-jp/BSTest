#ifndef KVSTORE_H
#define KVSTORE_H

/*
 * kvstore.h — Shared-Memory Key-Value Store
 * ==========================================
 * Dieser Header ist der "Vertrag" zwischen allen Teammitgliedern.
 * Die Signaturen hier duerfen NICHT veraendert werden.
 *
 * Wer implementiert was in kvstore.c:
 *   store_init()        -> Ehsan  (shmget, shmat, semget)
 *   store_cleanup()     -> Ehsan  (IPC_RMID cleanup)
 *   sem_lock/unlock()   -> Aryan  (semop)
 *   store_put/get/del() -> Aryan  (CRUD-Logik)
 *   store_get_version() -> Aryan  (fuer Zahras Pub/Sub)
 *   store_get_event()   -> Aryan  (fuer Zahras Pub/Sub)
 */

#include <stddef.h>

/* ── Groessen ─────────────────────────────────────────────────────── */
#define MAX_KEYS       64
#define MAX_KEY_LEN    64
#define MAX_VAL_LEN    256
#define MAX_EVENT_LEN  512

/*
 * entry_t: ein Eintrag im Store.
 * Das gesamte Array entry_t[MAX_KEYS] lebt im Shared Memory.
 * Alle Kindprozesse sehen dasselbe Array.
 *
 *   key        - Schluessel, z.B. "foo"
 *   value      - Wert, z.B. "bar"
 *   last_event - letztes Ereignis: "PUT:foo:bar" oder "DEL:foo:key_deleted"
 *   version    - wird bei jedem PUT/DEL um 1 erhoeht (Zahra braucht das)
 *   in_use     - 1 = Slot belegt, 0 = Slot frei
 */
typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VAL_LEN];
    char last_event[MAX_EVENT_LEN];
    int  version;
    int  in_use;
} entry_t;

/* ── Lifecycle ────────────────────────────────────────────────────── */
/* store_init():    VOR dem ersten fork() aufrufen (Ehsan) */
/* store_cleanup(): im SIGINT-Handler aufrufen     (Ehsan) */
void store_init(void);
void store_cleanup(void);

/* ── Semaphore ────────────────────────────────────────────────────── */
/* JEDER Zugriff auf den Store muss so aussehen:
 *   sem_lock();
 *   ... store lesen oder schreiben ...
 *   sem_unlock();
 * Ohne das entstehen Race Conditions (Aryan implementiert das).       */
void sem_lock(void);
void sem_unlock(void);

/* ── CRUD ─────────────────────────────────────────────────────────── */
/* Rueckgabe: 0 = Erfolg, -1 = Fehler (Aryan implementiert das)       */
int store_put(const char *key, const char *value);
int store_get(const char *key, char *out_value);
int store_del(const char *key);

/* ── Pub/Sub ──────────────────────────────────────────────────────── */
/* Zahra ruft das auf, Aryan implementiert es.                         */
int store_get_version(const char *key);
int store_get_event(const char *key, char *out_event);

#endif /* KVSTORE_H */
