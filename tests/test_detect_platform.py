import sys
from pathlib import Path
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
import detect_platform


class DetectPlatformTests(unittest.TestCase):
    def test_windows_64(self):
        with patch("platform.system", return_value="Windows"), \
             patch("platform.machine", return_value="AMD64"):
            self.assertEqual(detect_platform.get_platform(), "win64")

    def test_macos_arm64(self):
        with patch("platform.system", return_value="Darwin"), \
             patch("platform.machine", return_value="arm64"):
            self.assertEqual(detect_platform.get_platform(), "darwin-arm64")

    def test_linux_x86_64(self):
        with patch("platform.system", return_value="Linux"), \
             patch("platform.machine", return_value="x86_64"):
            self.assertEqual(detect_platform.get_platform(), "linux-x86-64")

    def test_raspberry_pi(self):
        cpuinfo = "model name\t: Raspberry Pi\n"
        with patch("platform.system", return_value="Linux"), \
             patch("platform.machine", return_value="armv7l"), \
             patch("pathlib.Path.read_text", return_value=cpuinfo):
            self.assertEqual(detect_platform.get_platform(), "raspberry-pi")


if __name__ == "__main__":
    unittest.main()
