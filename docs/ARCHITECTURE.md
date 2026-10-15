# Architecture
Native C++23 application; this is not a website.
```text
seed-client (native console UI) -- local TCP --> seed-server -- adapter --> MySQL
seed-admin (separate local operator console)
```
Layers: pure math and deterministic procedural generation; config/domain types; MySQL adapter or in-memory development mode; line-oriented local TCP protocol; native boot/loading UI.
The server is authoritative for identity and permissions. Network admin commands remain disabled until session-aware authorization is implemented. Never trust a client-supplied role.
