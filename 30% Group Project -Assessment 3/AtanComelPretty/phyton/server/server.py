import socket
import mysql.connector
from mysql.connector import Error
import time
import threading

# Update points for the user in the database
def update_database(username):
    try:
        # Connect to MySQL
        conn = mysql.connector.connect(
            host="192.168.32.2",  # Server IP updated to 192.168.32.2
            user="root",
            password="root",
            database="testdb"
        )
        if conn.is_connected():
            cursor = conn.cursor()
            query = f"""
                UPDATE user_points
                SET points = points + 1, datetime_stamp = NOW()
                WHERE username = %s;
            """
            cursor.execute(query, (username,))
            conn.commit()
            print(f"Database updated successfully for user: {username}")
        else:
            print("Connection to MySQL failed")
    except Error as e:
        print(f"Error: {e}")
    finally:
        if conn.is_connected():
            cursor.close()
            conn.close()

# Get points for the user from the database
def get_points(username):
    try:
        # Connect to MySQL
        conn = mysql.connector.connect(
            host="192.168.32.2",  # Server IP updated to 192.168.32.2
            user="root",
            password="root",
            database="testdb"
        )
        if conn.is_connected():
            cursor = conn.cursor()
            query = f"SELECT points FROM user_points WHERE username = %s;"
            cursor.execute(query, (username,))
            result = cursor.fetchone()
            if result:
                return f"User {username} has {result[0]} points."
            else:
                return f"User {username} not found."
        else:
            return "Connection to MySQL failed"
    except Error as e:
        return f"Error: {e}"
    finally:
        if conn.is_connected():
            cursor.close()
            conn.close()

# Function to periodically update points every 3 seconds
def periodic_update():
    while True:
        time.sleep(30)  # Wait for 3 seconds
        print("Performing scheduled database update...")
        update_database("Atan_Pretty")  # Update the database periodically for the same user

def main():
    # Set up the server
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_socket.bind(("0.0.0.0", 1025))  # Listening on all interfaces at port 1025
    print("Python Server running...")

    # Start the periodic update in a separate thread so it runs independently
    update_thread = threading.Thread(target=periodic_update)
    update_thread.daemon = True  # Ensures the thread will exit when the main program exits
    update_thread.start()

    while True:
        data, client_addr = server_socket.recvfrom(1024)
        message = data.decode().strip()
        print(f"Received message from client: {message}")

        # Handle requests based on message type
        if message.startswith("GET_POINTS:"):
            username = message[11:]  # Extract the username
            response = get_points(username)
        elif message.startswith("UPDATE:"):
            username = message[7:]  # Extract the username
            update_database(username)
            response = "Updated"
        else:
            response = "Error: Invalid request. Please send 'UPDATE:<username>' or 'GET_POINTS:<username>'."

        # Send the response back to the client
        server_socket.sendto(response.encode(), client_addr)

if __name__ == "__main__":
    main()
