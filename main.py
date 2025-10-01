

import socket
import struct
import time

LISTEN_ADDR = '127.0.0.1'
LISTEN_PORT = 8888
SEND_ADDR = '127.0.0.1'
SEND_PORT = 32766

# Variables to store incoming write-data (8 bytes each)
var_0006 = b'\x00' * 8
var_0008 = b'\x00' * 8
var_other = b'\x00' * 8

packet_counter = 0  # for responses

def is_all_zero(b: bytes) -> bool:
    return all(x == 0 for x in b)

def build_response(dev_id: bytes, address: int, data8: bytes, packet_num: int) -> bytes:

    if len(dev_id) != 4:
        # fallback: use 127.0.0.1
        dev_id = socket.inet_aton('127.0.0.1')
    header = bytes([(0xC << 4) | 0xC, (0x3 << 4) | 0x1])  # 0xCC, 0x31
    timestamp = int(time.time()) & 0xFFFFFFFF
    pkt_num = packet_num & 0xFFFF
    packed = (
        header +
        dev_id +
        struct.pack('>I', timestamp) +
        struct.pack('>H', pkt_num) +
        struct.pack('>H', address) +
        (data8 if len(data8) == 8 else (data8.ljust(8, b'\x00')[:8]))
    )
    return packed

def parse_request_packet(raw: bytes):
    """
    Parse incoming packet; return dict with keys if valid else None.
    Expect 22 bytes.
    """
    print(len(raw))
    if len(raw) < 22:
        return None
    b0, b1 = raw[0], raw[1]
    print(hex(b0))
    print(hex(b1))
    if (b0 >> 4) != 0xC or (b0 & 0x0F) != 0xC:
        return None  # not our protocol
    version = (b1 >> 4) & 0x0F
    print( "Version " + hex(version))
    ptype = b1 & 0x0F  # 0x0 - request, 0x1 - response
    dev_id = raw[2:6]
    print("Dev id " + dev_id.hex())
    timestamp = struct.unpack('>I', raw[6:10])[0]
    packet_number = struct.unpack('>H', raw[10:12])[0]
    addr = struct.unpack('>H', raw[12:14])[0]
    print("Addr " + hex(addr))
    data8 = raw[14:22]
    return {
        'version': version,
        'ptype': ptype,
        'dev_id': dev_id,
        'timestamp': timestamp,
        'packet_number': packet_number,
        'address': addr,
        'data8': data8,
        'raw': raw
    }

def main():
    global var_0006, var_0008, var_other, packet_counter

    # listening socket
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind((LISTEN_ADDR, LISTEN_PORT))
    s.settimeout(1.0)  # allow Ctrl+C

    # sending socket
    sender = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    print(f"Listening on {LISTEN_ADDR}:{LISTEN_PORT}, sending to {SEND_ADDR}:{SEND_PORT}. Ctrl-C to stop.")
    try:
        while True:
            try:
                raw, addr_from = s.recvfrom(4096)
            except socket.timeout:
                continue
            parsed = parse_request_packet(raw)
            if parsed is None:
                print("Received non-protocol or too short packet from", addr_from)
                continue
            print("I was parsed")
            # Only handle requests (ptype == 0) here
            print(hex(parsed['ptype']))
            # if parsed['ptype'] != 0x0:
            #     # ignore responses and other types
            #     continue
            print("And here&")
            address = parsed['address']
            data8 = parsed['data8']
            dev_id = parsed['dev_id']
            pktnum = parsed['packet_number']

            if is_all_zero(data8):
                # READ request
                # Determine which variable to answer with per user's mapping:
                # - read 0x0001 -> reply with var_0006
                # - read 0x4001 -> reply with var_0008
                if address == 0x0001:
                    reply_data = var_0006
                    print(f"Read request for 0x0001 -> replying with var_0006 ({var_0006.hex()})")
                elif address == 0x4001:
                    reply_data = var_0008
                    print(f"Read request for 0x4001 -> replying with var_0008 ({var_0008.hex()})")
                else:
                    reply_data = b'\x00' * 8
                    print(f"Read request for 0x{address:04x} -> replying with zeros")

                # Build and send response to configured SEND_ADDR:SEND_PORT
                packet_counter = (packet_counter + 1) & 0xFFFF
                resp = build_response(dev_id=dev_id, address=address, data8=reply_data, packet_num=packet_counter)
                sender.sendto(resp, (SEND_ADDR, SEND_PORT))
                # no reply to original sender (as per user's instruction)
            else:
                # WRITE request: store data into variable depending on address
                if address == 0x0006:
                    var_0006 = data8
                    print(f"Write request to 0x0006: stored {data8.hex()}")
                elif address == 0x0008:
                    var_0008 = data8
                    print(f"Write request to 0x0008: stored {data8.hex()}")
                else:
                    var_other = data8
                    print(f"Write request to 0x{address:04x}: stored in var_other {data8.hex()}")
                # do not send any reply on write
    finally:
        s.close()
        sender.close()

if __name__ == '__main__':
    main()

