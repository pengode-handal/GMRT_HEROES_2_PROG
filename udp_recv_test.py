import socket


HOST = "0.0.0.0"
PORT = 4210

receiver = socket.socket(
    socket.AF_INET,
    socket.SOCK_DGRAM,
)

receiver.bind((HOST, PORT))

print(f"Listening to {PORT}...")

while True:
    data, sender_address = receiver.recvfrom(256)

    message = data.decode(
        "utf-8",
        errors="replace",
    ).strip()

    print(
        f"from {sender_address}: {message}"
    )