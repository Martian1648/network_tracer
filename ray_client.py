from interpreter import run, ShellError, print_dict
import socket
import json
from PIL import Image
# Λ Imports 

# Set globals: the former is the size of the image in bytes, the latter is whether a given command was a GET request
img_size = 0
asked = False

def send_ack(s, msg):
    '''s = socket; msg = what is being acknowledged (must be shared with the server)'''
    data = dict()
    data['ack'] = msg
    s.sendall(json.dumps(data).encode())

def receive_exact(s, n):
    '''Receives n number of bytes on socket s'''
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
    '''Receives JSON message on socket s. size is defaulted to 4096. Also handles the contents of message'''
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

# retrieves the host and port
host = input("Enter ipaddr: \n")
port = int(input("Enter port number: \n"))

# Establishes connection
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))
receive(s)

while True:
    # Gets commands and handles errors
    try:
        line = input("> ")
        msg = run(line)
    except ShellError as e:
        print(f"error: {e}")
        continue
    except EOFError:
        msg = {"action": "QUIT"}
    
    # prints message back so you know what you sent
    print_dict(msg)
    s.sendall((json.dumps(msg)).encode())
    if(msg['action'] == 'QUIT'):
        break

    if(msg['action'] == 'GET'):
        asked = True
    
    # Get response from server, unless GET, it will contain a WARNING, which contains the number of bytes
    receive(s)

    # If the original command was a GET, resets, and bypasses receiving an image, since the server isn't sending one
    if asked:
        asked = False
        continue

    # Acknowledge that the number of bytes was received before the server sends the image
    send_ack(s, "WARNING")

    # receive bytes
    img_bytes = receive_exact(s, img_size)

    #Convert to image
    image = Image.frombytes("RGBA", (1280, 720), img_bytes)
    image.show()

    # Offer to save it
    if input("Save image? (y/n): ").strip().lower() == "y":
        name = input("Filename (without extension): ").strip()
        if not name:
            name = "output"
        image.save(f"{name}.png")
        print(f"Saved as {name}.png")

