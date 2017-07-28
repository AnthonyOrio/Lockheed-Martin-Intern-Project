import matplotlib.pyplot as plt
import math
import socket

# Initialize plotting coordinate lists
xPositions = [0]
yPositions = [0]
xWall = []
yWall = []

def main():
    # Initialize sockets
    HOST = ''
    PORT = 50000
    receiver = socket.socket()
    receiver.connect((HOST, PORT))

    # TODO - change the socket scheme to one that is Bluetooth compatible

    while True:
        msg = receiver.recv(1024)

        msgString = msg.decode('ascii')
        if msgString == "Done":
            receiver.close()
            break
        else:
            data = msgString.split(',')
            distance = float(data[0])
            bearing = float(data[1])
            sonarTime = float(data[2])

            addPoint(distance, bearing, sonarTime)

    # Display the output
    plt.plot(xPositions, yPositions, 'b-', xWall, yWall, 'r-')
    plt.title("DynaMITE Track and Canyon Plot")
    plt.grid(True)
    plt.legend(['Track Trace', 'Canyon Wall'])
    plt.show()

# Expect distance in inches, bearing in radians, and sonar time in microseconds
def addPoint(distanceTraveled, bearing, sonarResponseTime):
    # The most recent data point is used to project new points
    mostRecentX = xPositions[-1]
    mostRecentY = yPositions[-1]

    # New robot position is found via trig
    newX = mostRecentX + distanceTraveled * math.cos(bearing)
    newY = mostRecentY + distanceTraveled * math.sin(bearing)

    xPositions.append(newX)
    yPositions.append(newY)

    # Speed of sound is in inches per microsecond
    wallDistance = (sonarResponseTime / 2) * 979 * 12 / 1000000
    # We assume that the sonar is pointed directly to the right of the robot
    wallAngle = bearing - math.pi / 2
    newWallX = newX + wallDistance * math.cos(wallAngle)
    newWallY = newY + wallDistance * math.sin(wallAngle)

    xWall.append(newWallX)
    yWall.append(newWallY)

if __name__ == "__main__":
    main()
