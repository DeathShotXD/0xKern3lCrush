# Known Hashes — ProcessMonitorDriver.sys (CVE-2026-0828)

**For static analysis / verification only. DO NOT LOAD.**

From public sources (KOSEC / CERT VU#818729 / Tenable):

- Version 11.11.4.0  
  SHA256: 70bcec00c215fe52779700f74e9bd669ff836f594df92381cbfb7ee0568e7a8b

- Version 10.5.75.0  
  SHA256: 85d21ad0e0b43d122f3c9ec06036b08398635860c93d764f72fb550fb44cf786

Installer hash (Safetica endpoint client x64 — one possible source):  
SHA256: 9dbc82d61c0759c4db9862acd63408abd4664cd698b9d5669f9558a544133e3b

Tested OS: Windows 10 x64 (1903, 22H2 builds)

Additional info:
- Vulnerable IOCTL: 0xB822200C (arbitrary process termination via PID)
- Device symlink: \\.\STProcessMonitorDriver

Verification Note:
To verify your local artifact on Windows (PowerShell):
```powershell
Get-FileHash .\ProcessMonitorDriver.sys -Algorithm SHA256

Sources:
- Download ProcessMonitorDriver: https://github.com/KOSEC-LLC/BYOVD-Research/blob/main/Safetica/ProcessMonitorDriver.sys
- KOSEC original write-up (Nov 2025): https://kosec.io/2025/11/01/safetica-byovd.html 
- CERT VU#818729 (Jan 20, 2026): https://kb.cert.org/vuls/id/818729  
- CVE-2026-0828: https://www.cve.org/CVERecord?id=CVE-2026-0828  
- Tenable: https://www.tenable.com/cve/CVE-2026-0828
