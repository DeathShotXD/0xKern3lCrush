# 0xbyovd-patterns.md — Common BYOVD Patterns (2024–2026)

1. **Simple IOCTL Abuse** (Safetica-style)
   - Drop signed vulnerable driver → open device symlink → DeviceIoControl(PID) → PsTerminateProcess
   - Pros: Easy, low code
   - Cons: Driver must expose kill IOCTL; easier to detect/block

2. **Physical Memory R/W → Kernel Patch** (ThrottleStop / MedusaLocker-style)
   - Abuse MmMapIoSpace → read kernel base → patch function (e.g. NtAddAtom) → hook PsTerminateProcess
   - Pros: Kills PPL-protected processes reliably
   - Cons: Complex (KASLR bypass, phys addr translation), crash risk

3. **Other Primitives** (emerging)
   - Arbitrary kernel read/write → token stealing
   - Driver load via service creation → hide in temp paths
   - Rename driver to evade static signatures

Detection: Focus on driver load events + unusual DeviceIoControl + process kill spikes.

Expand with new cases from threat intel.
