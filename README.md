# 0xKern3lCrush

Windows BYOVD research on two real cases: the Safetica ProcessMonitorDriver
(CVE-2026-0828) and the ThrottleStop driver abused by MedusaLocker
(CVE-2025-7771). The repository documents the exposed interfaces, the
privileged outcome each one reaches, and how to detect the abuse. It ships
safe user-mode reconnaissance only: no kernel-mode code, no IOCTL invocation,
and no process termination routine.

<p align="center">
  <img src="logo.png" alt="0xKern3lCrush" width="340">
</p>

<br>

## What is 0xKern3lCrush?

Endpoint protection increasingly guards its own processes with protection
levels and restricted tokens, so attackers move the fight into the kernel
with Bring Your Own Vulnerable Driver. They drop a legitimate but vulnerable
signed driver, call a weak IOCTL handler, and reach a kernel primitive such
as arbitrary process termination or arbitrary memory read and write.

This repository studies that pattern on two public cases and turns each one
into detection and hardening material.

<br>

> The repository is a study, not a weapon. It carries no working exploit and
> no kernel-mode code.

## Why this matters

The Safetica and ThrottleStop cases are not theoretical. ThrottleStop.sys was
weaponized by MedusaLocker operators in real intrusions, and the pattern
generalizes across dozens of signed drivers abused between 2024 and 2026. A
driver that reaches kernel process termination or kernel memory write is a
direct path to silencing endpoint protection before a payload lands.

## Research contribution

The contribution here is a clear, reproducible writeup of each case and the
defensive material that follows from it:

- the exposed device and IOCTL surface for each driver
- the root cause in each handler
- the privileged outcome and its real-world use
- driver artifact hashes and public research mirrors
- detection and mitigation guidance

## Safetica ProcessMonitorDriver - CVE-2026-0828

- Publisher: Safetica Endpoint Client x64.
- Driver: `ProcessMonitorDriver.sys`.
- Affected versions: 10.5.75.0, 11.11.4.0.
- Published: January 2026 (KOSEC research originally November 2025).
- Root cause: the IOCTL handler does not validate the caller privilege and
  does not sanitize the input on the termination path.
- Outcome: an unprivileged caller can terminate arbitrary processes,
  including protected and system critical ones, from kernel context.
- Real-world use: a classic BYOVD primitive. The attacker drops the driver,
  terminates endpoint protection, and proceeds with the payload.
- Status: no vendor patch publicly confirmed as of February 2026. CERT
  VU#818729 was published on January 20, 2026.

Known hashes and mirrors are in [drivers/0xhashes.md](drivers/0xhashes.md).
Full notes are in
[research/0xsafetica-cve-2026-0828.md](research/0xsafetica-cve-2026-0828.md).

## ThrottleStop - CVE-2025-7771

MedusaLocker operators weaponized `ThrottleStop.sys`, the driver from the
TechPowerUp CPU throttling tool, in real intrusions, including a Brazilian
incident analyzed by Kaspersky in August 2025. It is a more advanced flow
than a direct IOCTL kill.

- Driver: `ThrottleStop.sys`, renamed to `ThrottleBlood.sys` by the
  attackers, signed by TechPowerUp with a 2020 DigiCert EV certificate.
- Root cause: the driver exposes IOCTLs that reach `MmMapIoSpace` with no
  access check, so a user-mode caller can read and write physical memory.
- Flow, analysis only:
  1. Load the renamed driver as a service and open `\\.\ThrottleStop`.
  2. Send the vulnerable IOCTLs (physical read `0x80006498`, write
     `0x8000649C`).
  3. Bypass KASLR by querying the kernel base through
     `NtQuerySystemInformation(SystemModuleInformation)`.
  4. Translate virtual to physical, often through a SuperFetch information
     leak.
  5. Overwrite a rarely used function with a hook.
  6. The hook reaches the process lookup and termination path and kills a
     hardcoded list of security processes.
  7. Restore the original bytes and deploy the ransomware.
- Outcome: kernel-level termination of protected security processes, which
  lets encryption proceed.
- Status: the vendor was preparing a patch through 2025; the driver was not
  on the Microsoft blocklist at the start.

Full notes are in
[research/0xthrottlestop-medusalocker.md](research/0xthrottlestop-medusalocker.md).
Primary source: Kaspersky Securelist, August 2025.

## Detection

- Driver load: watch for a random or renamed kernel service created from a
  `.sys` file followed immediately by a start. Event ID 7045 and, with
  service auditing on, 4697.
- Device access: handles opened on the device names each driver exposes,
  from an unexpected process.
- IOCTL: the termination and physical memory IOCTL codes, correlated with
  process termination of protected security processes.
- Blocklist: keep the Microsoft vulnerable driver blocklist and your WDAC
  policy current; the load fails where they are enforced.

Mitigation notes are in [docs/0xmitigations.md](docs/0xmitigations.md).

## Safe reconnaissance

`src/0xPoC.c` is read-only. It enumerates running processes through the
Toolhelp32 APIs and prints the names of common security products. It is the
first reconnaissance step and it performs no privileged action.

```powershell
# From a Visual Studio Developer Command Prompt
cl.exe /EHsc /W4 src/0xPoC.c
0xPoC.exe
```

## Repository structure

```
0xKern3lCrush/
|-- README.md
|-- LICENSE
|-- SECURITY.md
|-- src/
|   |-- 0xPoC.c          read-only process enumeration
|   +-- 0xtargets.h      target name list
|-- drivers/
|   +-- 0xhashes.md      artifact hashes and public mirrors
|-- research/
|   |-- 0xbyovd-patterns.md
|   |-- 0xsafetica-cve-2026-0828.md
|   +-- 0xthrottlestop-medusalocker.md
+-- docs/
    +-- 0xmitigations.md
```

## Limitations

- No working exploit is included, by design.
- The writeups are based on public disclosures and static analysis.
- Blocklist status is snapshot specific and changes over time.
- Version coverage reflects what was public at the time of writing.

## References

- Safetica ProcessMonitorDriver, CERT VU#818729 (2026-01-20).
- Kaspersky Securelist, AV killer exploiting ThrottleStop.sys (2025-08).
- Microsoft vulnerable driver blocklist.

## Responsible use

This project exists for research and defense. Load these drivers only on
systems you own or are authorized to test, and only in an isolated
environment with a snapshot to roll back to. Loading a vulnerable signed
driver or sending a crafted IOCTL without authorization is illegal in most
jurisdictions.
