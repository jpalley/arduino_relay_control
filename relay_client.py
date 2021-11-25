import socket
import time

class RelayClient:
    def __init__(self, ips, port = 2700):
        self.ips  = ips
        self.state  = [0xDD0000] * len(ips)
        self.port = port
        self.socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

    # Check that we can talk to all the clients
    def handshake_all(self):
        for ip in self.ips:
            self.socket.sendto(bytes.fromhex('AA0000'), (ip, self.port))

        ips_remaining = self.ips.copy()

        while len(ips_remaining) > 0:
            self.socket.settimeout(1)
            msg, (returnIP, returnPort) = self.socket.recvfrom(1)
            if (msg != b'\xbb'):
                raise("Invalid Handshake Msg")
            ips_remaining.remove(returnIP)

        return True

    def get_frames(self, ip_idx):
        self.socket.sendto(bytes.fromhex('CC0000'), (self.ips[ip_idx], self.port))

        self.socket.settimeout(1)
        msg, (returnIP, returnPort) = self.socket.recvfrom(2)
        if returnIP != self.ips[ip_idx]:
            raise "Get frame error - invalid return IP. Only call for one device at a time"
        
        return int.from_bytes(msg, "big")

    def set_relay(self, ip_idx, relay_idx, value, send_now = False):
        if value:
            self.state[ip_idx] |= (1 << relay_idx)
        else:
            self.state[ip_idx] &= ~(1 << relay_idx)
        if send_now: self.send_state(ip_idx)

    def set_all_on(self, ip_idx, send_now = False):
        self.state[ip_idx] = 0xDDFFFF
        if send_now: self.send_state(ip_idx)

    def set_all_off(self, ip_idx, send_now = False):
        self.state[ip_idx] = 0xDD0000
        if send_now: self.send_state(ip_idx)

    def send_state(self, ip_idx):
        self.socket.sendto(self.state[ip_idx].to_bytes(3, byteorder='big'), (self.ips[ip_idx], self.port))

