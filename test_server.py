import socket

localIP     = "127.0.0.1"
localPort   = 2700
bufferSize  = 1024
msg_counter = 0
msgFromServer       = "Hello UDP Client"
bytesToSend         = str.encode(msgFromServer)

# Create a datagram socket

UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

# Bind to address and ip
UDPServerSocket.bind((localIP, localPort))
print("UDP server up and listening")

# Listen for incoming datagrams
while(True):

    bytesAddressPair = UDPServerSocket.recvfrom(bufferSize)
    message = bytesAddressPair[0]
    address = bytesAddressPair[1]
    clientMsg = "Message from Client:{}".format(message)
    clientIP  = "Client IP Address:{}".format(address)
    print(clientMsg)
    print(message[0])
    if message[0] == 0xAA:
        UDPServerSocket.sendto(bytes.fromhex('BB00'), address)
    elif message[0] == 0xCC:
        print("MSG COUNTER")
        UDPServerSocket.sendto(msg_counter.to_bytes(2, byteorder='big'), address)
        msg_counter = 0
    elif message[0] == 0xDD:
        print(clientMsg)
        msg_counter += 1
    # Sending a reply to client
