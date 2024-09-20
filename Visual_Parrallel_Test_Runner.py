import subprocess
import time
import os
import glob
import random
import threading

# Change working directory to ./build
os.chdir("./build")

# =================================================================================  Variables
SERVER_EXECUTABLE   =   "./server"      # Location of Server Executable
CLIENT_EXECUTABLE   =   "./client"      # Location of Client Executable
LOG_FOLDER          =   "Logs"          # Name of the Log Folder

NUM_CLIENTS         =   8               # Number of clients
NUM_COMMANDS        =   50              # Number of draw commands per client
BOX_SIZE            =   10              # The size of the block

SERVER_PORT         =   8080            # Example port number
IP_ADDRESS          =   "127.0.0.1"     # The IP Address specified

X_COL               =  -350             # The Starting X position
Y_ROW               =   300             # The Starting Y position
X_SPACE             =   50              # The Starting X position
Y_SPACE             =   15              # The Starting Y position

COMMAND_INTERVAL    =   0.07            # The delay between each draw command (If you set to 0 commands may be lost)
LAUNCH_DELAY        =   5               # Time delay after the clients have been created
TERMINATION_DELAY   =   80              # Time delay before terminating the clients and server


# ================================================================================= 
#                                Log File Creation                                 
# =================================================================================
def create_log_file(nickname):
    os.makedirs(LOG_FOLDER, exist_ok=True)
    log_file = os.path.join(LOG_FOLDER, f"{nickname}.log")
    return open(log_file, "w")


# ================================================================================= 
#                                Log File Cleaning                                
# =================================================================================
def delete_existing_logs():
    for file in glob.glob(os.path.join(LOG_FOLDER, "*")):
        os.remove(file)


# ================================================================================= 
#                                Launch the Server                                 
# =================================================================================
def launch_server():
    log_file = create_log_file("server")

    server_process = subprocess.Popen(
        [SERVER_EXECUTABLE, "--port", str(SERVER_PORT)],
        stdin=subprocess.PIPE,
        stdout=log_file,
        stderr=log_file
    )
    return server_process, log_file


# ================================================================================= 
#                                Launch the Client                                 
# =================================================================================
def launch_client(nickname):
    log_file = create_log_file(nickname)

    client_process = subprocess.Popen(
        [CLIENT_EXECUTABLE, "--server"  , IP_ADDRESS, 
                            "--port"    , str(SERVER_PORT), 
                            "--nickname", nickname],
        stdin=subprocess.PIPE,
        stdout=log_file,
        stderr=log_file
    )

    return client_process, log_file, nickname


# ================================================================================= 
#                                   Random Colour                                  
# =================================================================================
def random_colour(client_process):
    r = random.randint(0, 255)
    g = random.randint(0, 255)
    b = random.randint(0, 255)
    client_process.stdin.write(f"colour {r} {g} {b}\n".encode())
    client_process.stdin.flush()


# ================================================================================= 
#                                   Draw Commands                                  
# =================================================================================
def send_draw_commands(client_process, index, nickname):
    try:
        random_colour(client_process)
        for i in range(NUM_COMMANDS):
            # Check if client process is still running
            if client_process.poll() is not None:
                print(f"Client process {nickname} has terminated.")
                return
            
            x_pos = X_COL + index * X_SPACE   # Adjust x position based on index
            y_pos = Y_ROW - i * Y_SPACE       # Adjust y position to decrement for downward movement

            client_process.stdin.write(f"draw {x_pos} {y_pos} {BOX_SIZE} {BOX_SIZE} fill\n".encode())
            client_process.stdin.flush()

            time.sleep(COMMAND_INTERVAL)
    except Exception as e:
        print(f"Error sending commands to client {nickname}: {e}")


def send_draw_commands_to_clients(client_processes):
    threads = []
    for index, (client_process, log_file, nickname) in enumerate(client_processes):
        thread = threading.Thread(target=send_draw_commands, args=(client_process, index, nickname))
        threads.append(thread)
        thread.start()

    for thread in threads:
        thread.join()


# ================================================================================= 
#                                   Main                                  
# =================================================================================
def main():
    try:
        # --------------------------------------  Deleting Logs
        delete_existing_logs()

        # -------------------------------------- Launching Server
        print(f"Launching server on port {SERVER_PORT}...")
        server_process, server_log = launch_server()

        # -------------------------------------- Launching Clients
        print("Launching clients...")
        client_processes = []
        for i in range(NUM_CLIENTS):
            time.sleep(1)
            client_process, client_log, nickname = launch_client(f"Client_{i}")
            client_processes.append((client_process, client_log, nickname))

        # -------------------------------------- Give time for clients to launch & move windows
        time.sleep(LAUNCH_DELAY)

        # -------------------------------------- Draw Commands
        print(f"Sending 'Draw Command' commands to clients...")
        send_draw_commands_to_clients(client_processes)


        # -------------------------------------- Give time to view results
        time.sleep(TERMINATION_DELAY)
        
        
        print(f"Testing over, Terminating ..... ")
        # -------------------------------------- Exit Clients
        for client_process, client_log, nickname in client_processes:
            try:
                if client_process.poll() is None:
                    client_process.stdin.write("exit\n".encode())
                    client_process.stdin.flush()
                client_process.stdin.close()
                client_process.wait()  # Ensure process has terminated
                client_log.close()
            except Exception as e:
                print(f"Error terminating client process {nickname}: {e}")

        # -------------------------------------- Exit Server
        try:
            if server_process.poll() is None:
                server_process.stdin.write("exit\n".encode())
                server_process.stdin.flush()
            server_process.stdin.close()
            server_process.wait()  # Ensure process has terminated
            server_log.close()
        except Exception as e:
            print(f"Error terminating server process: {e}")

    except Exception as e:
        print(f"An error occurred: {e}")


# ================================================================================= 
#                                   Exectue Main                                   
# =================================================================================
if __name__ == "__main__":
    main()
