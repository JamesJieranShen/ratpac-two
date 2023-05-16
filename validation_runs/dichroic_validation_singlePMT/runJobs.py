import multiprocessing
import subprocess
import time
import glob

def run_command(command):
    # Suppress stdout and stderr as we can just look at log files
    print(f"Running command: {command}", flush=True)
    start_time = time.time()
    subprocess.call(command, shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    end_time = time.time()
    print(f"Command {command} took {end_time - start_time:.2f} seconds.", flush=True)


if __name__ == "__main__":
    # file_names = ['macros/normalization_15deg.mac', 'macros/normalization_30deg.mac']
    file_names = glob.glob('macros/*.mac')

    with multiprocessing.Pool(processes=multiprocessing.cpu_count() - 2) as pool:
        commands = [f'rat {fname}' for fname in file_names]
        pool.map(run_command, commands)
