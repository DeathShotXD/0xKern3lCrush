#ifndef _0XTARGETS_H_
#define _0XTARGETS_H_

// Common EDR / AV / security product process names
// Used for educational reconnaissance research only
// Inspired by real-world BYOVD and ransomware target lists
// (Safetica, MedusaLocker-style kill lists, etc.)
// Feel free to extend for your lab/research targets

static const char* target_processes[] = {
    "MsMpEng.exe",          // Microsoft Defender
    "NisSrv.exe",           // Defender Network Inspection
    "AvastSvc.exe",         // Avast service
    "WRSA.exe",             // Webroot SecureAnywhere
    "csfalconservice.exe",  // CrowdStrike Falcon
    "SentinelAgent.exe",    // SentinelOne agent
    "cb.exe",               // Carbon Black (older naming)
    "CylanceSvc.exe",       // Cylance Protect
    "mcshield.exe",         // McAfee Endpoint Security
    "ccSvcHst.exe",         // Symantec / Norton
    "avp.exe",              // Kaspersky
    "ekrn.exe",             // ESET
    "bdagent.exe",          // Bitdefender
    "SAVService.exe",       // Sophos
    "mbamservice.exe",      // Malwarebytes
    "SafeticaAgent.exe",    // Safetica (relevant for CVE-2026-0828 research)
    // "cfp.exe",           // Comodo Firewall
    // "avgnt.exe",         // Avira
    // "avgui.exe",
    // "360tray.exe",       // 360 Total Security
    NULL                    // End of list marker — do not remove
};

#endif // _0XTARGETS_H_
