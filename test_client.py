##############################
# Demo on how to RelayClient
##########################
from relay_client import RelayClient
import time

ips = ['127.0.0.1']
frames_before_count = 300

c = RelayClient(ips) # Instantiate relayclient class

# Make sure we can talk to all the servers
if not c.handshake_all():
    raise("Can't handshake with server")

# Turn all relays off
c.set_all_off(0, True)

# Loop through and turn one relay one (i) and the last relay that was on, off (last_i)
i = 0
last_i = 15
frame_counter = 0

while (True):
    c.set_relay(0,i,True)
    c.set_relay(0,last_i, False)
    c.send_state(0)

    # Increment relay to turn on and loop around
    last_i = i
    i += 1
    if i > 15:
        i = 0

    # Output packet-loss info
    frame_counter += 1
    if frame_counter >= frames_before_count:
        # Try..catch in case there is packet loss in the get_frame command. If there is it will return an error with timeout.
        try:
            print("Frames sent: {}. Frames recv: {}".format(frame_counter, c.get_frames(0)))
        except:
            print("Error getting frame count!")
        frame_counter = 0

    time.sleep(0.03)
    