# Seed Engine — C++23 Game Mathematics Engine

A native C++23 program (not a website) for deterministic procedural generation and game mathematics. It includes a TCP backend server, console frontend client, local admin control panel, optional MySQL persistence, loading/boot screen, and CMake/CI setup.

## Components
- `seed-server`: backend server, registration/login commands, demo world generation.
- `seed-client`: native console client with boot/loading screen, register/login/logout and seeded world demo.
- `seed-admin`: local operator panel for user listing, account enable/disable and role changes.
- `seed_core`: configuration, PBKDF2-HMAC-SHA256 password hashing, procedural math, TCP protocol, and database adapter.
- `database/schema.sql`: users, sessions, modules, hierarchy nodes, stats, arrays, seeds, programs, math runs and audit log.
- `tests/`: C++23 unit tests.
- `.github/workflows/ci.yml`: Windows/Linux build and test job.

## Build
Requires CMake 3.25+, a C++23 compiler, OpenSSL development files, and Threads.

Linux/macOS:
```sh
cmake -S . -B build -DSEED_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Windows / Visual Studio 2022:
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

## MySQL mode
Install MySQL Server and MySQL/MariaDB C client development headers, create the `seed_engine` database, and apply `database/schema.sql`. Configure `config/seed.ini` from `config/seed.ini.example`, then configure with `-DSEED_WITH_MYSQL=ON`. Set `SEED_BOOTSTRAP_ADMIN_USER` and `SEED_BOOTSTRAP_ADMIN_PASSWORD` (12+ characters) in the environment for initial admin creation. Use a least-privilege database account; run migrations separately with a privileged account.

Without `SEED_WITH_MYSQL`, the app uses in-memory development storage and accounts are lost when the process exits. Multiple processes should use MySQL.

## Run
```sh
./build/seed-server config/seed.ini
./build/seed-client config/seed.ini
./build/seed-admin config/seed.ini
```
Executables are under `build/Debug`, `build/Release`, or `build/` depending on generator.

## Security status — development scaffold
Passwords are hashed using OpenSSL PBKDF2-HMAC-SHA256 and cryptographic random salts. The current TCP protocol is plain text and intended for localhost development only; do not expose it to the internet. Password entry in the console client is visible. Login is currently a stateless check and logout acknowledgement, not a persistent authenticated session. Network admin commands are disabled. The local admin utility must be protected by OS access controls; add an authenticated admin gate before shared deployment. Before production, implement server-issued session tokens with expiry/revocation, authorization middleware, TLS, rate limits, masked password input, audit logging, and security review.

The database adapter creates/uses the core account tables; apply `database/schema.sql` for the complete game-data schema. Verify MySQL mode against the exact server and connector version used in deployment.

## Deterministic generation
SplitMix64 is deterministic for a given seed and algorithm implementation. Persist seed, algorithm name/version and generator version. Changes to generation logic may alter output; maintain golden-seed tests.

## Scope
No proprietary source, schema or formulas from No Man's Sky or EVE Online are claimed. Those titles are high-level inspiration for procedural exploration and persistent science-fiction simulation. Example formulas are illustrative and need balancing and tests.
