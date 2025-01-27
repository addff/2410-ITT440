import socket
import datetime

SERVER_HOST = "python_server2"
SERVER_PORT = 2003

def main():
    # Connection timestamp
    connect_time = datetime.datetime.now().strftime('[%H:%M:%S]')
    print(f"{connect_time} Connecting to server...", flush=True)
    
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((SERVER_HOST, SERVER_PORT))
    
    # Request timestamp
    request_time = datetime.datetime.now().strftime('[%H:%M:%S]')
    print(f"{request_time} Sending GET request", flush=True)
    s.sendall(b"GET")
    
    data = s.recv(1024)
    
    # Response timestamp
    response_time = datetime.datetime.now().strftime('[%H:%M:%S]')
    print(f"{response_time} Server response: {data.decode('utf-8').strip()}", flush=True)
    
    s.close()

if __name__ == "__main__":
    main()
