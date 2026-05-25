/*
 * kvstore.c — Implementierung des Key-Value Stores
 * =================================================
 * AUFGABENVERTEILUNG IN DIESER DATEI:
 *
 *   Ehsan implementiert:
 *     - store_init()    (shmget, shmat, semget, semctl)
 *     - store_cleanup() (shmctl IPC_RMID, semctl IPC_RMID)
 *
 *   Aryan implementiert:
 *     - sem_lock() / sem_unlock()
 *     - store_put() / store_get() / store_del()
 *     - store_get_version() / store_get_event()
 *
 * Warum sind beide in einer Datei?
 *   Weil store_init() den Shared-Memory-Pointer anlegt (Ehsan),
 *   den Aryan dann in seinen CRUD-Funktionen benutzt.
 *   Sie muessen sich also kurz absprechen wie der Pointer heisst.
 *   Vorschlag: static entry_t *store = NULL;
 *              static int shm_id = -1;
 *              static int sem_id = -1;
 */

#include "../include/kvstore.h"
#include <stdio.h>
#include <string.h>
#include <sys/sem.h>


/* ── Gemeinsame Variablen (Ehsan legt diese an, Aryan benutzt sie) ── */
 static entry_t *store = NULL; 
 static int      shm_id = -1;  
 static int      sem_id = -1;  

/* ════════════════════════════════════════════════════════════════════
 * EHSAN — Shared Memory + Semaphore Lifecycle
 * ════════════════════════════════════════════════════════════════════ */

void store_init(void) {
    /* TODO: Ehsan
     *
     * Schritt 1: Shared Memory anlegen
     *   shm_id = shmget(IPC_PRIVATE, sizeof(entry_t) * MAX_KEYS, IPC_CREAT | 0600);
     *
     * Schritt 2: In den Adressraum einhaengen
     *   store = (entry_t *) shmat(shm_id, NULL, 0);
     *
     * Schritt 3: Nullen (alle in_use = 0)
     *   memset(store, 0, sizeof(entry_t) * MAX_KEYS);
     *
     * Schritt 4: Semaphore anlegen
     *   sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
     *   semctl(sem_id, 0, SETVAL, 1);  <- Startwert 1 = freigegeben
     */
    printf("[store] init (stub)\n");
}

void store_cleanup(void) {
    /* TODO: Ehsan
     *
     * Shared Memory entfernen:
     *   shmdt(store);
     *   shmctl(shm_id, IPC_RMID, NULL);
     *
     * Semaphore entfernen:
     *   semctl(sem_id, 0, IPC_RMID);
     *
     * Wichtig: ohne cleanup bleiben die Ressourcen im Kernel!
     * Test: ipcs -m und ipcs -s zeigen verbleibende Ressourcen.
     */
    printf("[store] cleanup (stub)\n");
}

/* ════════════════════════════════════════════════════════════════════
 * ARYAN — Semaphore + CRUD
 * ════════════════════════════════════════════════════════════════════ */

void sem_lock(void) {
    /* TODO: Aryan*/
       struct sembuf op = {0, -1, 0};
        semop(sem_id, &op, 1);
     
}

void sem_unlock(void) {
     /*TODO: Aryan*/
        struct sembuf op = {0, +1, 0};
        semop(sem_id, &op, 1);
     
}

int store_put(const char *key, const char *value) {
    /* TODO: Aryan*/
     
      sem_lock();
		   for (int i = 0; i < MAX_KEYS; i++) {

        if (store[i].in_use == 1 &&
            strcmp(store[i].key, key) == 0)
        {
            strncpy(store[i].value, value, MAX_VAL_LEN);

            store[i].version++;

            snprintf(store[i].last_event,
                     MAX_EVENT_LEN,
                     "PUT:%s:%s",
                     key,
                     value);

            sem_unlock();
            return 0;
        }
    }

    /* اگر key وجود نداشت -> ذخیره جدید */
    for (int i = 0; i < MAX_KEYS; i++) {

        if (store[i].in_use == 0)
        {
            store[i].in_use = 1;

            strncpy(store[i].key, key, MAX_KEY_LEN);
            strncpy(store[i].value, value, MAX_VAL_LEN);

            store[i].version = 1;

            snprintf(store[i].last_event,
                     MAX_EVENT_LEN,
                     "PUT:%s:%s",
                     key,
                     value);

            sem_unlock();
            return 0;
        }
    }

    sem_unlock();
    return -1;
}
      

         

     
      /*1. Suche ob key schon existiert -> ueberschreiben
      2. Sonst: freien Slot suchen (in_use == 0)
      3. key, value kopieren (strncpy)
      4. version++ und last_event setzen: "PUT:key:value"
      5. in_use = 1*/
    
  

int store_get(const char *key, char *out_value) {
    /* TODO: Aryan*/
     
      sem_lock();
     /* Suche key im Array, kopiere value in out_value
      sem_unlock();
      return 0 wenn gefunden, -1 wenn nicht*/


    sem_lock();

    for (int i = 0; i < MAX_KEYS; i++) {

        if (store[i].in_use == 1 &&
            strcmp(store[i].key, key) == 0)
        {
            strcpy(out_value, store[i].value);

            sem_unlock();
            return 0;
        }
    }

    sem_unlock();
    return -1;
}
     
    (void)key;
    (void)out_value;
    return -1;
}

int store_del(const char *key) {
    /* TODO: Aryan*/
     /*
      WICHTIG: in_use NICHT auf 0 setzen!
      Stattdessen: version++ und last_event = "DEL:key:key_deleted"
      Zahra erkennt das DEL-Event dadurch.
      Erst DANACH in_use = 0.
     */
	sem_lock();
	   for (int i = 0; i < MAX_KEYS; i++) {

        if (store[i].in_use == 1 &&
            strcmp(store[i].key, key) == 0)
        {
            store[i].version++;

            snprintf(store[i].last_event,
                     MAX_EVENT_LEN,
                     "DEL:%s:key_deleted",
                     key);

            store[i].in_use = 0;

            sem_unlock();
            return 0;
        }
    }

    sem_unlock();
    return -1;
}
    

int store_get_version(const char *key) {
    /* TODO: Aryan — version-Feld des Eintrags zurueckgeben */
	 sem_lock();

    for (int i = 0; i < MAX_KEYS; i++) {

        if (store[i].in_use == 1 &&
            strcmp(store[i].key, key) == 0)
        {
            int v = store[i].version;

            sem_unlock();
            return v;
        }
    }

    sem_unlock();
    return -1;
}
   

int store_get_event(const char *key, char *out_event) {
    /* TODO: Aryan — last_event in out_event kopieren */
	    sem_lock();

    for (int i = 0; i < MAX_KEYS; i++) {

        if (store[i].in_use == 1 &&
            strcmp(store[i].key, key) == 0)
        {
            strcpy(out_event, store[i].last_event);

            sem_unlock();
            return 0;
        }
    }

    sem_unlock();
    return -1;
}
   
