import socket


class PoseUdpSender:
    def __init__(self, robot_ip, robot_port):
        self.robot_address = (robot_ip, robot_port)
        self.socket = socket.socket(
            socket.AF_INET,
            socket.SOCK_DGRAM,
        )
        self.sequence = 0

    def _next_sequence(self):
        self.sequence = (self.sequence + 1) % 4_294_967_296
        return self.sequence

    def send_pose(self, marker_id, tx, tz):
        sequence = self._next_sequence()

        message = (
            f"P,{sequence},{marker_id},"
            f"{tx:.4f},{tz:.4f}\n"
        )

        self.socket.sendto(
            message.encode("ascii"),
            self.robot_address,
        )

        return message.strip()

    def send_no_marker(self):
        sequence = self._next_sequence()
        message = f"N,{sequence}\n"

        self.socket.sendto(
            message.encode("ascii"),
            self.robot_address,
        )

        return message.strip()

    def close(self):
        self.socket.close()