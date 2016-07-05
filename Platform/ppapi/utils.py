import subprocess

# Helper to execute command
def execute_command(cmd, cwd=None):
    p = subprocess.Popen(cmd, cwd=cwd)
    p.wait()