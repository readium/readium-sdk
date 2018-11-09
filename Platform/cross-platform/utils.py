import subprocess
import os.path

# Helper to execute command
def execute_command(cmd, cwd=None):
    cwd_abs_path = None

    if cwd is not None:
        cwd_abs_path = os.path.abspath(cwd)

    p = subprocess.Popen(cmd, cwd=cwd_abs_path)
    p.wait()