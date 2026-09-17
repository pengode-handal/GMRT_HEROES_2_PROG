import math
import socket

from mdns_resolver import DEFAULT_SERVICE_TYPE, resolve_udp_endpoint


MAX_SEQUENCE = 2**32


def encode_pose(sequence: int, marker_id: int, tx: float, tz: float) -> str:
    if not math.isfinite(tx) or not math.isfinite(tz) or tz <= 0.0:
        raise ValueError("pose values must be finite and tz must be positive")

    return f"P,{sequence},{marker_id},{tx:.4f},{tz:.4f}\n"


def encode_no_marker(sequence: int) -> str:
    return f"N,{sequence}\n"


class PoseUdpSender:
    def __init__(
        self,
        robot_host: str,
        robot_port: int,
        service_type: str = DEFAULT_SERVICE_TYPE,
    ):
        self.robot_host = robot_host
        self.robot_port = robot_port
        self.service_type = service_type
        self.robot_address = resolve_udp_endpoint(
            robot_host,
            robot_port,
            service_type,
        )
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sequence = 0

    @property
    def destination(self) -> str:
        return f"{self.robot_host} -> {self.robot_address[0]}:{self.robot_address[1]}"

    def _next_sequence(self) -> int:
        self.sequence = (self.sequence + 1) % MAX_SEQUENCE
        return self.sequence

    def send_pose(self, marker_id: int, tx: float, tz: float) -> str:
        message = encode_pose(self._next_sequence(), marker_id, tx, tz)
        self._send(message)
        return message.strip()

    def send_no_marker(self) -> str:
        message = encode_no_marker(self._next_sequence())
        self._send(message)
        return message.strip()

    def _send(self, message: str) -> None:
        try:
            self.socket.sendto(message.encode("ascii"), self.robot_address)
        except OSError:
            self.robot_address = resolve_udp_endpoint(
                self.robot_host,
                self.robot_port,
                self.service_type,
            )
            self.socket.sendto(message.encode("ascii"), self.robot_address)

    def close(self) -> None:
        self.socket.close()
