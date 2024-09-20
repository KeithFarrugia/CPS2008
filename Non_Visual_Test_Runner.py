import re
import subprocess
import time
import os
import glob
import random
import sys

# Change working directory to ./build
os.chdir("./build")

# =================================================================================  Variables
SERVER_EXECUTABLE           =   "./server"      # Location of Server Executable
CLIENT_EXECUTABLE           =   "./client"      # Location of Client Executable
LOG_FOLDER                  =   "Logs"          # Name of the Log Folder

NUM_CLIENTS                 =   100             # Number of clients
NUM_COMMANDS                =   50              # Number of draw commands per client
BOX_SIZE_MIN                =   5               # Minimum size of the block
BOX_SIZE_MAX                =   20              # Maximum size of the block

SERVER_PORT                 =   8080            # Example port number
IP_ADDRESS                  =   "127.0.0.1"     # The IP Address specified

COMMAND_INTERVAL            =   0.07            # The delay between each draw command (If you set to 0 commands may be lost)
LAUNCH_DELAY                =   5               # Time delay after the clients have been created
TERMINATION_DELAY           =   5               # Time delay before terminating the clients and server
TERMINATION_DELAY_VISUAL    =   50              # Time delay before terminating visual Client

X_MIN                       =   -350            # Minimum x position
X_MAX                       =   350             # Maximum x position
Y_MIN                       =   -300            # Minimum y position
Y_MAX                       =   300             # Maximum y position


# ================================================================================= 
#                                Hash Value Confirmation                                 
# =================================================================================
def verify_hashes(logfile):
    pattern = re.compile(r'\[Client_\d+\s*\]:\s*([a-f0-9]{64})')
    
    with open(logfile, 'r') as file:
        hashes = {pattern.search(line).group(1) for line in file if pattern.search(line)}
    
    return len(hashes) == 1

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
def launch_client(nickname, log_file, test = True):
    if(test == True):
        client_process = subprocess.Popen(
            [CLIENT_EXECUTABLE, "--server"  , IP_ADDRESS, 
                                "--port"    , str(SERVER_PORT), 
                                "--nickname", nickname,
                                "--test"],
            stdin=subprocess.PIPE,
            stdout=log_file,
            stderr=log_file
        )
        return client_process, nickname

    else:
        client_process = subprocess.Popen(
            [CLIENT_EXECUTABLE, "--server"  , IP_ADDRESS, 
                                "--port"    , str(SERVER_PORT), 
                                "--nickname", nickname],
            stdin=subprocess.PIPE,
            stdout=log_file,
            stderr=log_file
        )
        return client_process, nickname

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
def send_draw_commands_to_clients(client_processes):
    try:
        command_counts = {nickname: 0 for _, nickname in client_processes}
        total_commands = NUM_COMMANDS * len(client_processes)
        # -------------------------------------- First set the colours for each Client
        for client_process,_ in client_processes:
            # Check if client process is still running
            if client_process.poll() is not None: 
                print(f"Client process has terminated.")
                return

            random_colour(client_process)
        
        # -------------------------------------- Send random draw commands to each client
        for _ in range(NUM_COMMANDS):
            for client_process, nickname in client_processes:
                # Check if client process is still running
                if client_process.poll() is not None:
                    print(f"Client process {nickname} has terminated.")
                    return

                # Generate random coordinates and size
                x_pos = random.randint(X_MIN, X_MAX)
                y_pos = random.randint(Y_MIN, Y_MAX)
                size = random.randint(BOX_SIZE_MIN, BOX_SIZE_MAX)

                # Send draw command
                client_process.stdin.write(f"draw {x_pos} {y_pos} {size} {size} fill\n".encode())
                client_process.stdin.flush()

                # Update command count
                command_counts[nickname] += 1
                time.sleep(COMMAND_INTERVAL)

            # Calculate overall progress
            overall_progress = sum(command_counts.values()) / total_commands * 100
            loading_bar = "#" * int(overall_progress / 2) + "-" * (50 - int(overall_progress / 2))
            
            # Print loading bar with carriage return to overwrite previous line
            sys.stdout.write(f"\rOverall Progress: [{loading_bar}] {overall_progress:.1f}%")
            sys.stdout.flush()
            
    except Exception as e:
        print(f"Error sending commands to clients: {e}")

# ================================================================================= 
#                                   Main                                  
# =================================================================================
def main():
    try:
        # --------------------------------------  Deleting Logs
        delete_existing_logs()

        # -------------------------------------- Create Log File
        server_log_file         = create_log_file("Server")
        client_log_file         = create_log_file("Client")
        visual_client_log_file  = create_log_file("Visual_Client")

        # -------------------------------------- Launching Server
        print(f"Launching server on port {SERVER_PORT}...")
        server_process = launch_server(server_log_file)

        # -------------------------------------- Launching Clients
        print("Launching clients...")
        client_processes = []
        for i in range(NUM_CLIENTS):
            time.sleep(1)
            client_process, nickname = launch_client(f"Client_{i}", client_log_file)
            client_processes.append((client_process, nickname))

        # -------------------------------------- Give time for clients to launch & move windows
        time.sleep(LAUNCH_DELAY)

        # -------------------------------------- Draw Commands
        print(f"Sending 'Draw Command' commands to clients...")
        send_draw_commands_to_clients(client_processes)

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

        time.sleep(LAUNCH_DELAY);
        
        print(f"\nLaunching Visual Client ..... ")
        # -------------------------------------- Launching & Exit Visual Client
        visual_client_process, vs_nickname = launch_client(f"Client_Visual", visual_client_log_file, False)

        time.sleep(TERMINATION_DELAY_VISUAL)
        
        print(f"\nTerminating Visual Window ..... ")
        try:
            if visual_client_process.poll() is None:
                visual_client_process.stdin.write("exit\n".encode())
                visual_client_process.stdin.flush()
            visual_client_process.stdin.close()
            visual_client_process.wait()  # Ensure process has terminated
        except Exception as e:
            print(f"Error terminating client process {vs_nickname}: {e}")
        

        print(f"\nKilling Server ..... ")
        # -------------------------------------- Exit Server
        try:
            if server_process.poll() is None:
                server_process.stdin.write("exit\n".encode())
                server_process.stdin.flush()
            server_process.stdin.close()
            server_process.wait()  # Ensure process has terminated
        except Exception as e:
            print(f"Error terminating server process: {e}")

        server_log_file.close()
        client_log_file.close()
        visual_client_log_file.close()

    except Exception as e:
        print(f"An error occurred: {e}")


    # -------------------------------------- Exit Server
    print("Successfull -> All hash values match" if verify_hashes(client_log_file.name) else "Failure -> Not all hash match")
# ================================================================================= 
#                                   Execute Main                                   
# =================================================================================
if __name__ == "__main__":
    main()
