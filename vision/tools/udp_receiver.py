import argparse
import socket


def main() -> None:
    parser = argparse.ArgumentParser(description="Print incoming GMRT UDP packets.")
    parser.add_argument("--host", default="0.0.0.0")
    parser.add_argument("--port", type=int, default=4210)
    args = parser.parse_args()

    receiver = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    receiver.bind((args.host, args.port))
    print(f"Listening on {args.host}:{args.port}")

    try:
        while True:
            data, sender = receiver.recvfrom(256)
            message = data.decode("ascii", errors="replace").strip()
            print(f"{sender[0]}:{sender[1]} -> {message}")
    except KeyboardInterrupt:
        print("Stopped by user.")
    finally:
        receiver.close()


if __name__ == "__main__":
    main()

