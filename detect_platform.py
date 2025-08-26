#!/usr/bin/env python3
"""Derive the PLATFORM string used by the Sockets library.

This script inspects the current operating system and machine
architecture and prints the corresponding PLATFORM identifier used
by the library's build system.
"""
from __future__ import annotations
import platform
from pathlib import Path

def get_platform() -> str:
    system = platform.system().lower()
    machine = platform.machine().lower()

    if system == "windows":
        return "win64" if "64" in machine else "win32"

    if system == "darwin":
        if machine == "arm64":
            return "darwin-arm64"
        if machine in ("x86_64",):
            return "darwin-x86-64"

    if system == "linux":
        if machine in ("x86_64", "amd64"):
            return "linux-x86-64"
        if machine in ("i386", "i686", "x86"):
            return "linux-x86-32"
        if machine.startswith("arm") or machine.startswith("aarch64"):
            try:
                cpuinfo = Path("/proc/cpuinfo").read_text().lower()
                if "raspberry pi" in cpuinfo:
                    return "raspberry-pi"
            except OSError:
                pass

    return f"{system}-{machine}"


if __name__ == "__main__":
    print(get_platform())
