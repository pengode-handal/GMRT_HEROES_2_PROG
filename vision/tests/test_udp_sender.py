import sys
from pathlib import Path
import types
import unittest
from unittest.mock import patch


sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from mdns_resolver import resolve_udp_endpoint, service_instance_name
from udp_sender import encode_no_marker, encode_pose


class UdpProtocolTest(unittest.TestCase):
    def test_pose_packet(self):
        self.assertEqual(encode_pose(7, 1, 0.1, 0.6), "P,7,1,0.1000,0.6000\n")

    def test_no_marker_packet(self):
        self.assertEqual(encode_no_marker(8), "N,8\n")

    def test_invalid_depth_is_rejected(self):
        with self.assertRaises(ValueError):
            encode_pose(1, 1, 0.0, -1.0)

    def test_literal_ip_does_not_need_mdns(self):
        self.assertEqual(
            resolve_udp_endpoint("127.0.0.1", 4210),
            ("127.0.0.1", 4210),
        )

    def test_mdns_service_instance_name(self):
        self.assertEqual(
            service_instance_name(
                "gmrt-heroes-2.local",
                "_gmrt-udp._udp.local.",
            ),
            "gmrt-heroes-2._gmrt-udp._udp.local.",
        )

    def test_mdns_service_discovery(self):
        calls = {}

        class FakeInfo:
            port = 4210

            def parsed_addresses(self, _ip_version):
                return ["192.168.1.50"]

        class FakeZeroconf:
            def get_service_info(self, service_type, name, timeout):
                calls["service_type"] = service_type
                calls["name"] = name
                calls["timeout"] = timeout
                return FakeInfo()

            def close(self):
                calls["closed"] = True

        fake_module = types.ModuleType("zeroconf")
        fake_module.IPVersion = types.SimpleNamespace(V4Only=4)
        fake_module.Zeroconf = FakeZeroconf

        with patch.dict(sys.modules, {"zeroconf": fake_module}):
            endpoint = resolve_udp_endpoint("gmrt-heroes-2.local", 4210)

        self.assertEqual(endpoint, ("192.168.1.50", 4210))
        self.assertEqual(calls["service_type"], "_gmrt-udp._udp.local.")
        self.assertEqual(
            calls["name"],
            "gmrt-heroes-2._gmrt-udp._udp.local.",
        )
        self.assertTrue(calls["closed"])


if __name__ == "__main__":
    unittest.main()
