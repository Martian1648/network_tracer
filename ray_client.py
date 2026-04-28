from interpreter import run, ShellError, print_dict
import socket
import json
from PIL import Image
img_size = 0
asked = False

def send_ack(s, msg):
    data = dict()
    data['ack'] = msg
    s.sendall(json.dumps(data).encode())

def receive_exact(s, n):
    chunks = []
    remaining = n
    while remaining > 0:
        chunk = s.recv(min(remaining, 4096))
        if not chunk:
            raise ConnectionError("socket closed mid-read")
        chunks.append(chunk)
        remaining -= len(chunk)
    return b"".join(chunks)

def receive(s, size=4096):
    res = s.recv(size).decode()
    data = json.loads(res)
    type = data['type']
    if(type=='WARNING'):
        global img_size 
        img_size = data['contents']
    elif(type=='MESSAGE'):
        print_dict(data)
    elif(type=='ANSWER'):
        print_dict(data)

host, port = '127.0.0.1', 5005
host = input("Enter ipaddr: \n")
port = int(input("Enter port number: \n"))

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))
receive(s)
while True:
    try:
        line = input("> ")
        msg = run(line)
    except ShellError as e:
        print(f"error: {e}")
        continue
    except EOFError:
        # Ctrl+D: quit gracefully
        msg = {"action": "QUIT"}
    
    print_dict(msg)
    s.sendall((json.dumps(msg)).encode())
    if(msg['action'] == 'QUIT'):
        break
    if(msg['action'] == 'GET'):
        asked = True
    receive(s)
    if asked:
        asked = False
        continue
    send_ack(s, "WARNING")
    img_bytes = receive_exact(s, img_size)
    image = Image.frombytes("RGBA", (1280, 720), img_bytes)
    image.show()

