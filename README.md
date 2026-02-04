# 💀 0xKern3lCrush-M4te-CVE-2026-0828
# Windows BYOVD Research & Endpoint Recon Notes

<img src="logo.png" alt="0xKern3lCrush Logo" width="1024">

**Strictly educational / security research repository.**  
Goal: Document and understand Bring-Your-Own-Vulnerable-Driver (BYOVD) techniques via public disclosures — **zero working exploits included**.

This repo collects:
- Safe user-mode reconnaissance code (process enumeration via Toolhelp32 APIs)
- Static analysis artifacts and notes on real BYOVD cases
- No kernel-mode code, no IOCTL invocation logic, no process termination routines

### ⚠️ Critical Ethical & Legal Warning (read before anything else)
- This repo is **NOT** an exploit delivery mechanism.
- **Do NOT** load the included driver sample on any system — even in a lab — without isolated VM + snapshot rollback.
- Loading vulnerable signed drivers or sending malicious IOCTLs without explicit authorization is a felony in most countries (computer fraud/abuse, unauthorized access, etc.).
- Use **only** on systems you own or have **written permission** to test.
- Even "research" activity can brick machines, corrupt OS installs, or trigger irreversible EDR alerts.

### Motivation
EDR/AV products increasingly protect their own processes (PPL, protected process light, restricted tokens).  
Attackers bypass via **BYOVD**: drop a legitimate-but-vulnerable signed driver → abuse weak IOCTL handlers → achieve kernel-level arbitrary process kill / memory r/w / etc.

This repo helps red/blue teams:
- Spot BYOVD patterns in malware (MedusaLocker, Qilin, Storm-2603, etc.)
- Understand weak driver design flaws
- Build better detections (driver blocklists, WDAC rules, IOCTL monitoring)

### Key Files
- `src/0xPoC.c`  
  Safe, read-only process enumeration — lists common EDR/AV/EDR service names. First recon step before any hypothetical advanced technique.

- `drivers/0xhashes.md`   
  **Artifact Identification** — Contains SHA256 verification and links to public research mirrors. This repo does **not** host binary drivers.

### Studied Case: CVE-2026-0828 — Safetica ProcessMonitorDriver.sys
- **Published**: January 2026 (KOSEC research originally Nov 2025)
- **Driver**: `ProcessMonitorDriver.sys` (Safetica Endpoint Client x64)
- **Affected versions**: 10.5.75.0, 11.11.4.0
- **Known hashes** → see `drivers/0xhashes.md`
- **Vulnerability**: Unprivileged user can abuse exposed IOCTL handler(s) to terminate arbitrary processes (including protected/system critical ones)
- **Root cause**: Lack of caller privilege validation + improper input sanitization on IOCTL path
- **Impact**: Kernel-level process termination (PsTerminateProcess style) → blind Safetica DLP/monitoring → data exfil, ransomware staging
- **Real-world context**: Classic BYOVD primitive — attackers drop driver → kill EDR → proceed with payload
- **Status**: No vendor patch publicly confirmed as of Feb 2026; CERT VU#818729 published Jan 20, 2026

Detailed notes → `research/0xsafetica-cve-2026-0828.md`

### Studied Case: CVE-2025-7771 — ThrottleStop.sys Abuse by MedusaLocker Ransomware
MedusaLocker (RaaS since 2019) operators have weaponized **ThrottleStop.sys** (TechPowerUp CPU throttling tool driver) in real-world attacks, most notably a Brazilian incident (analyzed by Kaspersky, Aug 2025). This shows a more advanced BYOVD flow compared to simple IOCTL kills.

- **Driver**: `ThrottleStop.sys` (renamed to `ThrottleBlood.sys` by attackers), signed by TechPowerUp (DigiCert EV, 2020 cert)
- **Vulnerability**: CVE-2025-7771 — exposes IOCTLs allowing arbitrary physical memory read/write via `MmMapIoSpace`. No proper access checks → user-mode app can patch kernel memory and invoke ring-0 functions.
- **Exploitation Flow (concept/analysis only)**:
  1. Load renamed driver (`ThrottleBlood.sys`) as service → create device `\\.\ThrottleStop`
  2. Use user-mode tool (`All.exe` or similar) to send vulnerable IOCTLs (e.g., codes like 0x80006498 read / 0x8000649C write)
  3. Bypass KASLR: query kernel base via `NtQuerySystemInformation(SystemModuleInformation)`
  4. Translate virtual → physical addresses (often via SuperFetch info leak technique)
  5. Read/write kernel memory → overwrite rarely-used function (e.g., `NtAddAtom`) with shellcode hook
  6. Hook jumps to `PsLookupProcessById` + `PsTerminateProcess` → mass-kill AV/EDR processes (hardcoded list: MsMpEng.exe, CSFalconService.exe, ekrn.exe, etc.)
  7. Restore original code to avoid crashes → deploy MedusaLocker variant
- **Impact**: Kernel-god mode termination of protected processes → disable CrowdStrike, Bitdefender, Kaspersky, McAfee, Defender, etc. → ransomware encryption succeeds.
- **Real-world context**: Seen in RDP credential theft → Mimikatz → pass-the-hash lateral → AV kill → MedusaLocker drop. Circulating since ~Oct 2024; targeted Russia, Brazil, Europe.
- **Status**: Vendor preparing patch (as of 2025); not always on MS blocklist initially.

Detailed technical breakdown → `research/0xthrottlestop-medusalocker.md`  
Primary source: Kaspersky Securelist (Aug 2025) — https://securelist.com/av-killer-exploiting-throttlestop-sys/117026/

### Other Cases
Broader BYOVD trends → dozens of signed vulnerable drivers abused 2024–2026 (see `research/0xbyovd-patterns.md`)

### Build & Run (safe recon only)
```powershell
# From Developer Command Prompt (VS)
cl.exe /EHsc /W4 src/0xPoC.c
0xPoC.exe
