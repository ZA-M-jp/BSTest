/*
 * test_kvstore.c — Selbsttest fuer Aryan
 * ========================================
 * Dieses Programm testet nur kvstore.c — unabhaengig vom Server.
 *
 * Bauen und ausfuehren (nachdem Aryan und Ehsan fertig sind):
 *   gcc tests/test_kvstore.c src/kvstore.c -I include -o test_kv
 *   ./test_kv
 *
 * Erwartete Ausgabe:
 *   [OK] store_put foo=bar
 *   [OK] store_get foo -> bar
 *   [OK] store_put foo=baz (ueberschreiben)
 *   [OK] store_get foo -> baz
 *   [OK] store_del foo
 *   [OK] store_get foo -> nicht gefunden
 *   Alle Tests bestanden.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/kvstore.h"

int main(void) {
    store_init();

    char out[MAX_VAL_LEN];

    /* Test 1: PUT */
    if (store_put("foo", "bar") == 0)
        printf("[OK] store_put foo=bar\n");
    else {
        printf("[FAIL] store_put\n"); return 1;
    }

    /* Test 2: GET */
    if (store_get("foo", out) == 0 && strcmp(out, "bar") == 0)
        printf("[OK] store_get foo -> %s\n", out);
    else {
        printf("[FAIL] store_get erwartet 'bar', bekam '%s'\n", out); return 1;
    }

    /* Test 3: PUT ueberschreibt */
    store_put("foo", "baz");
    if (store_get("foo", out) == 0 && strcmp(out, "baz") == 0)
        printf("[OK] store_put foo=baz (ueberschreiben)\n");
    else {
        printf("[FAIL] ueberschreiben fehlgeschlagen\n"); return 1;
    }

    /* Test 4: DEL */
    store_del("foo");
    if (store_get("foo", out) == -1)
        printf("[OK] store_del foo -> nicht mehr da\n");
    else {
        printf("[FAIL] foo sollte geloescht sein\n"); return 1;
    }

    /* Test 5: version steigt */
    store_put("x", "1");
    int v1 = store_get_version("x");
    store_put("x", "2");
    int v2 = store_get_version("x");
    if (v2 > v1)
        printf("[OK] version steigt bei PUT (%d -> %d)\n", v1, v2);
    else {
        printf("[FAIL] version sollte gestiegen sein\n"); return 1;
    }

    store_cleanup();
    printf("\nAlle Tests bestanden.\n");
    return 0;
}
