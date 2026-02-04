# CVE-2025-7771 — ThrottleStop.sys Abuse in MedusaLocker Ransomware Attacks

## Summary
MedusaLocker ransomware (RaaS since Sep 2019) uses BYOVD to disable AV/EDR via physical memory r/w abuse in legitimate ThrottleStop.sys driver. Kaspersky documented in Aug 2025 (Brazil incident).

## Timeline
- Driver signed: 2020 (TechPowerUp)
- Abuse observed: ~Oct 2024 onward
- Kaspersky analysis: Aug 6, 2025
- CVE assigned: CVE-2025-7771

## Technical Details
- Driver: `ThrottleStop.sys` → renamed `ThrottleBlood.sys` by attackers
- Device: `\\.\ThrottleStop`
- Vulnerable IOCTLs: Allow phys mem read/write via `MmMapIoSpace` (no privilege/sanitization checks)
- Hash example (vulnerable renamed driver): SHA-256: 16f83f056177c4ec24c7e99d01ca9d9d6713bd0497eeedb777a3ffefa99c97f0

## Exploitation Concept (analysis only — NO CODE)
1. Load driver as service → open device
2. Gather kernel base (NtQuerySystemInformation + LoadLibrary for offsets)
3. Virtual-to-physical translation (SuperFetch info leak)
4. Read/write phys mem → patch kernel function (e.g., NtAddAtom → shellcode hook)
5. Hook calls PsLookupProcessById + PsTerminateProcess on AV PIDs
6. Loop + kill (targets: MsMpEng.exe, CSFalconService.exe, bdagent.exe, etc.)
7. Restore patch → execute ransomware

## Impact
Kernel-level AV/EDR kill → bypass PPL protections → ransomware deploys freely. Seen with RDP → Mimikatz → PTH lateral movement.

## Mitigation (as of early 2026)
- Block ThrottleStop.sys load (WDAC / vulnerable driver blocklist)
- Monitor service creation for suspicious .sys
- Detect phys mem access patterns / unusual DeviceIoControl
- Strong RDP hardening (MFA, restricted access)
- EDR self-defense + behavioral rules for kernel hooks

## References
- Kaspersky Securelist: https://securelist.com/av-killer-exploiting-throttlestop-sys/117026/
- CVE-2025-7771: https://www.cve.org/CVERecord?id=CVE-2025-7771
- Related: Hive Pro / Acronis reports on MedusaLocker campaigns
