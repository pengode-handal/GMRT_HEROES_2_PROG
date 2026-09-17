from __future__ import annotations

import argparse
from pathlib import Path
import sys
import time


sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from udp_sender import PoseUdpSender


SCENARIOS = [
    ("center and far", 0.00, 1.00, 1.0),
    ("target on the right", 0.10, 0.60, 1.0),
    ("target on the left", -0.10, 0.60, 1.0),
    ("large right error", 0.30, 0.50, 1.0),
    ("target reached", 0.00, 0.22, 1.0),
]


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Send repeatable test poses to ESP32.")
    parser.add_argument("--robot-host", default="gmrt-heroes-2.local")
    parser.add_argument("--port", type=int, default=4210)
    parser.add_argument("--marker-id", type=int, default=1)
    parser.add_argument("--rate", type=float, default=20.0)
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    sender = PoseUdpSender(args.robot_host, args.port)
    interval = 1.0 / args.rate

    print(f"UDP destination: {sender.destination}")

    try:
        for name, tx, tz, duration in SCENARIOS:
            print(f"Scenario: {name} (tx={tx:.2f}, tz={tz:.2f})")
            deadline = time.monotonic() + duration

            while time.monotonic() < deadline:
                sender.send_pose(args.marker_id, tx, tz)
                time.sleep(interval)

        print("Scenario: no marker")
        sender.send_no_marker()
        time.sleep(0.5)

        print("Scenario: UDP timeout (no packets for 0.5 seconds)")
        time.sleep(0.5)
    finally:
        sender.close()


if __name__ == "__main__":
    main()
