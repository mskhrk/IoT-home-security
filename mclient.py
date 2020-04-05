import socket
import cv2
import numpy

def recvall(sock, count):
    buf = b''
    while count:
        newbuf = sock.recv(count)
        if not newbuf: return None
        buf += newbuf
        count -= len(newbuf)
    return buf

TCP_IP = "127.0.0.1"
TCP_PORT = 8000

sock = socket.socket()
sock.connect((TCP_IP, TCP_PORT))
while 1:
    length = recvall(sock,16)
    try:
        stringData = recvall(sock, int(length))
        data = numpy.fromstring(stringData, dtype='uint8')
        decimg=cv2.imdecode(data,1)
        cv2.imshow('CLIENT2',decimg)
        cv2.waitKey(10)
    except ValueError:
        print("ValueError")

sock.close()
cv2.destroyAllWindows()