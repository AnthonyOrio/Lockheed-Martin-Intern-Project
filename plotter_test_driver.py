import math
import random
import socket

def main():
    # TODO - open a socket
    sender = socket.socket()
    HOST = ''
    PORT = 50000
    sender.bind((HOST, PORT))
    sender.listen(1)

    i = 0
    while True:
        client, addr = sender.accept()
        print("Got client at {0}".format(addr))
        break

    while True:
        if i < 10:
            dist = random.randint(2, 5)
            bearing = random.randint(-45, 45) * math.pi / 180
            time = random.randint(200, 600)
            messageString = "{0},{1},{2}".format(dist, bearing, time)
            client.send(messageString.encode('ascii'))
            i += 1
            print("Sent data: " + messageString)
        else:
            messageString = "Done"
            client.send(messageString.encode('ascii'))
            client.close()
            print("Sent end and closed socket")
            break

if __name__ == "__main__":
    main()
