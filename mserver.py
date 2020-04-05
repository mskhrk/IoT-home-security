import socket
import sys
import traceback
import cv2
import numpy
import time
from threading import Thread

def main():
    start_server()
def start_server():
    global online_user ,sok
    online_user=0
    sok=0
    host = "127.0.0.1"
    port = 8000 # arbitrary non-privileged port
    soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    soc.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    print("Socket created")
    try:
        soc.bind((host, port))
    except:
        print("Bind failed. Error : " + str(sys.exc_info()))
        sys.exit()
    soc.listen(3) # queue up to 6 requests
    print("Socket now listening")
    # infinite loop- do not reset for every requests
    try:
        Thread(target=videoThread, args=()).start()
    except:
        print("videoThread did not start.")
    #time.sleep(1)
    while True:
        connection, address = soc.accept()
        ip, port = str(address[0]), str(address[1])
        online_user+=1
        print("Connected with " + ip + ":" + port)
        try:
            Thread(target=clientThread, args=(connection, ip, port)).start()
        except:
            print("Thread did not start.")
            traceback.print_exc()
            soc.close()
    sys.exit()
def clientThread(connection, ip, port, max_buffer_size = 5120):
    is_active = True
    global front_data,stringData,sok,online_user,sok2
    while True:
        try:
            #time.sleep(0.033)
            if sok>0:
                sok-=1
                connection.send( front_data.encode())
                connection.send( stringData )
        except ConnectionResetError: 
            online_user-=1
            break


def videoThread():
    capture = cv2.VideoCapture(0)
    print("Start Camera")
    ret, frame = capture.read()
    encode_param=[int(cv2.IMWRITE_JPEG_QUALITY),90]
    is_active = True
    global front_data,stringData,sok,online_user,sok2
    while ret :
        if sok<=0:
            sok=0
            result, imgencode = cv2.imencode('.jpg', frame, encode_param)
            data = numpy.array(imgencode)
            stringData = data.tostring()
            front_data=str(len(stringData)).ljust(16)
            ret, frame = capture.read()
            sok=1
            sok=online_user
        #print(online_user)

if __name__ == "__main__":
    main()