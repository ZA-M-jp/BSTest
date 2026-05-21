# Multi-Client Key-Value Server in C

BS 26 · TH Köln · Praktikum 2026

---

## Wer macht was

| Person   | Dateien                         | Aufgabe                                |
|----------|---------------------------------|----------------------------------------|
| Sami     | Alle (Skeleton)                 | Struktur, Docker, Build, Header, Stubs |
| Aryan    | `src/kvstore.c`                 | CRUD-Logik, Semaphore                  |
| Ehsan    | `src/kvstore.c`, `src/main.c`  | Shared Memory, Fork                    |
| Mohannad | `src/main.c`                    | TCP-Socket, Befehlsparser              |
| Zahra    | `src/pubsub.c`                  | Pub/Sub, Praesentation                 |

---

## Ordnerstruktur

```
bs-praktikum/
├── include/           <- Header (.h) — Schnittstellen, nie aendern!
│   ├── kvstore.h      <- Key-Value Store API
│   └── pubsub.h       <- Pub/Sub API
├── src/               <- Implementierungen (.c)
│   ├── kvstore.c      <- Aryan + Ehsan
│   ├── pubsub.c       <- Zahra
│   └── main.c         <- Ehsan + Mohannad
├── tests/             <- Selbsttests
│   └── test_kvstore.c <- Aryan testet seinen Store hier
├── docker/
│   └── Dockerfile.dev <- Entwicklungsumgebung (Ubuntu + gcc + SSH)
├── docker-compose.yml <- startet Container mit einem Befehl
├── CMakeLists.txt     <- Build-Konfiguration
└── .gitignore         <- ignoriert build/ und IDE-Dateien
```

---

## Schnellstart (3 Schritte)

### Schritt 1 — Container starten
```bash
docker compose up -d
```

### Schritt 2 — Terminal im Container oeffnen
```bash
docker compose exec dev bash
cd /project
```

### Schritt 3 — Bauen und starten
```bash
cmake -B build .
cmake --build build
./build/server
```

---

## CLion Remote einrichten (einmalig)

1. Settings → Build, Execution, Deployment → Toolchains
2. "+" → Remote Host
3. Host: `localhost` · Port: `2222` · User: `user` · Passwort: `bs2026`
4. CMake Profile → Remote-Toolchain auswaehlen

---

## Testen (nach Meilenstein II)

```bash
# Terminal 1
./build/server

# Terminal 2
nc localhost 5555
PUT foo bar      # Antwort: PUT:foo:bar
GET foo          # Antwort: GET:foo:bar
DEL foo          # Antwort: DEL:foo:key_deleted

# Terminal 3 (gleichzeitig)
nc localhost 5555
GET foo          # sieht dieselben Daten dank Shared Memory
```

## Testen Pub/Sub (nach Meilenstein III)

```bash
# Terminal 2
SUB foo          # Antwort: SUB:foo:no_value_yet

# Terminal 3
PUT foo hello    # Terminal 2 bekommt automatisch: PUT:foo:hello
DEL foo          # Terminal 2 bekommt automatisch: DEL:foo:key_deleted
```

---

## Selbsttest kvstore (Aryan)

```bash
cmake --build build --target test_kv
./build/test_kv
# Erwartete Ausgabe: "Alle Tests bestanden."
```

---

## Git-Workflow

```bash
# Eigenen Branch erstellen (NIE direkt auf main pushen)
git checkout main && git pull
git checkout -b feat/mein-feature

# Arbeiten, dann:
git add .
git commit -m "feat: store_put implementiert"
git push origin feat/mein-feature
# -> Pull Request auf GitHub aufmachen
```

**Branches:**
- `main` — nur stabiler, getesteter Code
- `feat/skeleton` — Sami (Push #1)
- `feat/kvstore-impl` — Aryan (#2)
- `feat/shm-fork` — Ehsan (#3)
- `feat/tcp-parser` — Mohannad (#4)
- `feat/pubsub` — Zahra (#5)
