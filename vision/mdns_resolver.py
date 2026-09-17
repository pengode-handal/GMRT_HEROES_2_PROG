from __future__ import annotations

import ipaddress
import socket


DEFAULT_SERVICE_TYPE = "_gmrt-udp._udp.local."


def service_instance_name(hostname: str, service_type: str) -> str:
    host_label = hostname.rstrip(".")
    if host_label.lower().endswith(".local"):
        host_label = host_label[:-6]

    normalized_type = service_type.rstrip(".") + "."
    return f"{host_label}.{normalized_type}"


def _literal_ipv4(hostname: str) -> str | None:
    try:
        address = ipaddress.ip_address(hostname)
    except ValueError:
        return None

    return str(address) if address.version == 4 else None


def _resolve_with_zeroconf(
    hostname: str,
    port: int,
    service_type: str,
    timeout_s: float,
) -> tuple[str, int]:
    try:
        from zeroconf import IPVersion, Zeroconf
    except ImportError as error:
        raise RuntimeError(
            "Paket 'zeroconf' belum terpasang. Jalankan: "
            "python -m pip install -r requirements.txt"
        ) from error

    normalized_type = service_type.rstrip(".") + "."
    instance_name = service_instance_name(hostname, normalized_type)
    zeroconf = Zeroconf()

    try:
        info = zeroconf.get_service_info(
            normalized_type,
            instance_name,
            timeout=max(1, int(timeout_s * 1000)),
        )
        if info is None:
            raise RuntimeError(
                f"Layanan mDNS {instance_name} tidak ditemukan. "
                "Pastikan ESP32 dan laptop berada pada Wi-Fi yang sama."
            )

        addresses = info.parsed_addresses(IPVersion.V4Only)
        if not addresses:
            raise RuntimeError(
                f"Layanan mDNS {instance_name} tidak memiliki alamat IPv4."
            )

        return addresses[0], info.port or port
    finally:
        zeroconf.close()


def resolve_udp_endpoint(
    hostname: str,
    port: int,
    service_type: str = DEFAULT_SERVICE_TYPE,
    timeout_s: float = 3.0,
) -> tuple[str, int]:
    if not hostname:
        raise ValueError("hostname tidak boleh kosong")
    if not 1 <= port <= 65535:
        raise ValueError("port harus berada pada rentang 1..65535")

    literal = _literal_ipv4(hostname)
    if literal is not None:
        return literal, port

    mdns_error: Exception | None = None
    if hostname.rstrip(".").lower().endswith(".local"):
        try:
            return _resolve_with_zeroconf(
                hostname,
                port,
                service_type,
                timeout_s,
            )
        except (RuntimeError, OSError) as error:
            mdns_error = error

    try:
        results = socket.getaddrinfo(
            hostname,
            port,
            family=socket.AF_INET,
            type=socket.SOCK_DGRAM,
        )
        return results[0][4][0], port
    except socket.gaierror as error:
        detail = f" ({mdns_error})" if mdns_error is not None else ""
        raise RuntimeError(
            f"Hostname {hostname!r} tidak dapat ditemukan{detail}."
        ) from error
