# Security Checklist
- Password storage uses OpenSSL PBKDF2-HMAC-SHA256 with per-user cryptographic random salt.
- Consider Argon2id and have password hashing cost reviewed for production.
- Do not expose plain TCP to the internet; add TLS first.
- Add rate limits, password masking, persistent sessions, expiry and revocation.
- Add server-side authorization checks for every privileged command.
- Keep admin operations local until authorization is implemented.
- Use prepared SQL statements; keep secrets out of source control.
- Separate migration credentials from runtime credentials.
- Add audit records for role/account changes and test backups/restores.
