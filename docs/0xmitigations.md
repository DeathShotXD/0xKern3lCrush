# 0xmitigations.md — Defending Against BYOVD & Vulnerable Driver Abuse (2026 View)

**Core Goal**: Prevent loading & exploitation of vulnerable signed drivers (Safetica ProcessMonitorDriver.sys, ThrottleStop.sys, etc.) used in BYOVD attacks by ransomware (MedusaLocker), nation-state actors, and commodity malware.

**Current Status (Feb 2026)**:
- Microsoft Vulnerable Driver Blocklist: Enabled by default on Win11 22H2+ (via Core Isolation / Memory Integrity / Smart App Control). Updated via Windows Update or major releases (1-2× per year). Includes many abused drivers but **not** guaranteed to have ProcessMonitorDriver.sys yet (no public addition confirmed).
- Safetica CVE-2026-0828: No vendor patch released as of Feb 2026 (per CERT VU#818729, Tenable, KOSEC). Uninstall/remove Safetica if unused.
- ThrottleStop.sys (CVE-2025-7771): Vendor (TechPowerUp) reportedly preparing patch; block via custom rules.

## Recommended Mitigations (Layered Defense)

### 1. Enable Microsoft Built-in Protections (Quick Wins)
- **Memory Integrity (HVCI)**: Turn on in Windows Security → Device Security → Core Isolation. Blocks many driver loads by enforcing kernel code integrity.
- **Microsoft Vulnerable Driver Blocklist**:
  - Path: Windows Security → Device Security → Core Isolation → Memory Integrity details → "Microsoft Vulnerable Driver Blocklist" toggle (grayed out? → ensure updates installed + HVCI on).
  - Enforced automatically with HVCI, Smart App Control, or S mode.
  - Updates via Windows Update (occasional servicing releases) or manual via App Control for Business.
- **Smart App Control**: On clean Win11 22H2+ installs → blocks untrusted apps/drivers proactively.

### 2. Windows Defender Application Control (WDAC) — Gold Standard for BYOVD
- Use **App Control for Business** (formerly WDAC) to enforce custom or Microsoft-recommended policies.
- Steps to apply Microsoft recommended vulnerable driver blocklist:
  1. Download latest policy XML/binary from: https://learn.microsoft.com/en-us/windows/security/application-security/application-control/app-control-for-business/design/microsoft-recommended-driver-block-rules
  2. Convert to binary (if needed) via PowerShell: `ConvertFrom-CIPolicy -XmlFilePath .\SiPolicy.xml -BinaryFilePath .\SiPolicy.p7b`
  3. Deploy via Group Policy / Intune / MDM → Computer Configuration → Administrative Templates → System → Device Guard → Deploy Windows Defender Application Control.
  4. Audit mode first → review Event Viewer (Microsoft-Windows-CodeIntegrity/Operational) → then enforce.
- Advanced: Create **kernel-mode-only strict policy** → deny all unsigned/unknown drivers by default, allow-list only required hardware/software drivers (e.g., via supplemental policies).
- Tools: WDAC Policy Wizard[](https://webapp-wdac-wizard.azurewebsites.net/), HotCakeX Harden-Windows-Security repo for BYOVD-focused templates.

### 3. Monitoring & Detection (EDR / SIEM Rules)
- Alert on:
  - New kernel services created (`sc create` with .sys binPath)
  - DeviceIoControl to suspicious devices (`\\.\STProcessMonitorDriver`, `\\.\ThrottleStop`)
  - Driver loads from unusual paths (Event ID 6 — DriverFrameworks-UserMode)
  - Physical memory access patterns (rare, but indicative of ThrottleStop-style r/w)
- Microsoft Defender for Endpoint ASR rule: "Block abuse of exploited vulnerable signed drivers" — auto-updates with known abused drivers.
- Hunt for renamed drivers (ThrottleBlood.sys pattern).

### 4. Hardening Basics (Reduce Attack Surface)
- Least privilege: Limit local admin rights (BYOVD often needs admin to load drivers).
- Restrict driver installation: Group Policy → Prevent installation of devices not described by other policy settings.
- Remove unnecessary endpoint agents: If Safetica not critical → uninstall (removes ProcessMonitorDriver.sys).
- Patch & harden RDP/credential exposure (common entry for MedusaLocker → BYOVD chain).
- VM/snapshot rollback for research: Never load samples on host.

### 5. Long-Term / Enterprise
- Zero-trust drivers: Explicitly allow-list kernel drivers only (WDAC kernel-mode policy).
- Submit suspicious drivers: https://www.microsoft.com/en-us/wdsi/driversubmission (helps expand blocklist).
- Monitor threat intel: KOSEC, Kaspersky, CERT for new BYOVD disclosures.

**Bottom Line (2026)**: Blocklist + HVCI covers ~70-80% of known cases. For full BYOVD resistance → strict WDAC kernel policy is king. Audit before enforce — wrong policy = bluescreens.

Stay locked down. Pwn defense, not destruction.
