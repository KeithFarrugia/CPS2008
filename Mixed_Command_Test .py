import re
import subprocess
import time
import os
import glob
import threading

# Change working directory to ./build
os.chdir("./build")

# =================================================================================  Variables
SERVER_EXECUTABLE           = "./server"            # Location of Server Executable
CLIENT_EXECUTABLE           = "./client"            # Location of Client Executable
LOG_FOLDER                  = "Logs"                # Name of the Log Folder

SERVER_PORT                 = 8080                  # Example port number
IP_ADDRESS                  = "127.0.0.1"           # The IP Address specified

COMMAND_INTERVAL            = 1                     # The delay between each draw command (If you set to 0 commands may be lost)
LAUNCH_DELAY                = 5                     # Time delay after the clients have been created
TERMINATION_DELAY           = 20                    # Time delay before terminating the clients and server
TERMINATION_DELAY_VISUAL    = 20                    # Time delay before terminating visual Client


# =================================================================================
#                                Hash Value Confirmation
# =================================================================================
def verify_hashes(logfile1, logfile2):
    pattern = re.compile(r'\[Client_\d+\s*\]:\s*([a-f0-9]{64})')
    
    hashes_1 = set()
    hashes_2 = set()

    with open(logfile1, 'r') as file:
        for line in file:
            match = pattern.search(line)
            if match:
                hashes_1.add(match.group(1))
    
    with open(logfile2, 'r') as file:
        for line in file:
            match = pattern.search(line)
            if match:
                hashes_2.add(match.group(1))

    return hashes_1 == hashes_2


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
def launch_server(log_file):
    server_process = subprocess.Popen(
        [SERVER_EXECUTABLE, "--port", str(SERVER_PORT)],
        stdin=subprocess.PIPE,
        stdout=log_file,
        stderr=log_file
    )
    return server_process


# =================================================================================
#                                Launch the Client
# =================================================================================
def launch_client(nickname, log_file, test=True):
    if test:
        client_process = subprocess.Popen(
            [CLIENT_EXECUTABLE, "--server", IP_ADDRESS,
             "--port", str(SERVER_PORT),
             "--nickname", nickname,
             "--test"],
            stdin=subprocess.PIPE,
            stdout=log_file,
            stderr=log_file
        )
    else:
        client_process = subprocess.Popen(
            [CLIENT_EXECUTABLE, "--server", IP_ADDRESS,
             "--port", str(SERVER_PORT),
             "--nickname", nickname],
            stdin=subprocess.PIPE,
            stdout=log_file,
            stderr=log_file
        )
    return client_process, nickname


# =================================================================================
#                                   Random Colour
# =================================================================================
def send_command(client_process, command):
    if client_process.poll() is None:  # Ensure client process is running
        try:
            client_process.stdin.write((command + "\n").encode())
            client_process.stdin.flush()
            time.sleep(COMMAND_INTERVAL)
        except Exception as e:
            print(f"Error sending command '{command}' to client: {e}")
    else:
        print("Client process is not running.")


# =================================================================================
#                                   Draw Commands
# =================================================================================
def send_draw_client_1(client_process, nickname):
    try:
        send_command(client_process, "colour 0 200 200")
        send_command(client_process, "tool rectangle")
        send_command(client_process, "draw 0 0 100 100 fill")
        send_command(client_process, "select 0")
        send_command(client_process, "draw -100 0 50 50 fill")  # Edit at the same time
        send_command(client_process, 'delete 0')                # Delete at the same time

    except Exception as e:
        print(f"Error sending commands to client {nickname}: {e}")


def send_draw_client_2(client_process, nickname):
    try:
        send_command(client_process, "colour 255 0 0")
        send_command(client_process, "tool text")
        send_command(client_process, 'draw -100 -100 "Hello"')
        send_command(client_process, "select 0")
        send_command(client_process, 'draw -150 -100 "X"')      # Edit at the same time
        send_command(client_process, 'delete 0')                # Delete at the same time

    except Exception as e:
        print(f"Error sending commands to client {nickname}: {e}")


# =================================================================================
#                                Send Draw Commands                                
# =================================================================================
def send_draw_commands_to_clients(client_1, nickname_1, client_2, nickname_2):
    threads = []

    thread = threading.Thread(target=send_draw_client_1, args=(client_1, nickname_1))
    threads.append(thread)
    thread.start()

    thread = threading.Thread(target=send_draw_client_2, args=(client_2, nickname_2))
    threads.append(thread)
    thread.start()

    for thread in threads:
        thread.join()


# =================================================================================
#                                   Main
# =================================================================================
def main():
    try:
        client_processes = []

        # --------------------------------------  Deleting Logs
        delete_existing_logs()

        # -------------------------------------- Create Log Files
        server_log_file = create_log_file("Server")
        non_visual_client_1_log = create_log_file("NV_Client_1")
        non_visual_client_2_log = create_log_file("NV_Client_2")
        visual_client_log_file_log = create_log_file("Visual_Client")

        # -------------------------------------- Launching Server
        print(f"Launching server on port {SERVER_PORT}...")
        server_process = launch_server(server_log_file)
        time.sleep(1)

        # -------------------------------------- Launching Clients
        print("Launching clients...")
        nv_client_1, nv_client_1_nickname       = launch_client("Client_1",     non_visual_client_1_log,    True)
        time.sleep(1)
        nv_client_2, nv_client_2_nickname       = launch_client("Client_2",     non_visual_client_2_log,    True)
        time.sleep(1)
        visual_client, visual_client_nickname   = launch_client("Client_VS",    visual_client_log_file_log, False)

        client_processes.append((nv_client_1, nv_client_1_nickname))
        client_processes.append((nv_client_2, nv_client_2_nickname))
        client_processes.append((visual_client, visual_client_nickname))

        # -------------------------------------- Give time for clients to launch & move windows
        time.sleep(LAUNCH_DELAY)

        print(f"Sending 'Draw Command' commands to clients...")
        send_draw_commands_to_clients(
            nv_client_1, nv_client_1_nickname,
            nv_client_2, nv_client_2_nickname
        )





        # -------------------------------------- Give time to view results
        time.sleep(TERMINATION_DELAY)
        print(f"\nTesting over, Terminating Clients ..... ")

        # -------------------------------------- Exit Clients
        for client_process, nickname in client_processes:
            try:
                if client_process.poll() is None:
                    client_process.stdin.write("exit\n".encode())
                    client_process.stdin.flush()
                client_process.stdin.close()
                client_process.wait()  # Ensure process has terminated
            except Exception as e:
                print(f"Error terminating client process {nickname}: {e}")




        # -------------------------------------- Exit Server
        print(f"\nKilling Server ..... ")
        try:
            if server_process.poll() is None:
                server_process.stdin.write("exit\n".encode())
                server_process.stdin.flush()
            server_process.stdin.close()
            server_process.wait()  # Ensure process has terminated
        except Exception as e:
            print(f"Error terminating server process: {e}")

        print("Successfull -> All hash values match" 
              if verify_hashes(non_visual_client_1_log.name, non_visual_client_2_log.name) 
              else "Failure -> Not all hash match")

        # -------------------------------------- Close Log Files
        server_log_file.close()
        non_visual_client_1_log.close()
        non_visual_client_2_log.close()
        visual_client_log_file_log.close()

    except Exception as e:
        print(f"An error occurred: {e}")


# =================================================================================
#                                   Execute Main
# =================================================================================
if __name__ == "__main__":
    main()
