import subprocess
import os.path

# Helper to execute command
def execute_command(cmd, cwd=None):
    p = subprocess.Popen(cmd, cwd=os.path.abspath(cwd))
    p.wait()