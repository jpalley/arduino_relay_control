from relay_client import RelayClient
import time

ips = ['127.0.0.1']
c = RelayClient(ips)
if not c.handshake_all():
    raise("Can't handshake with server")

c.set_all_off(0)
i = 0
last_i = 15
frame_counter = 0
while (True):
    c.set_relay(0,i,True)
    c.set_relay(0,last_i, False)
    c.send_state(0)

    last_i = i
    i += 1
    if i > 15:
        i = 0

    frame_counter += 1
    if frame_counter >= 30:
        frame_counter = 0
        print(c.get_frames(0))

    time.sleep(0.03)
    