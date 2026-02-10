#!/usr/bin/env python3
"""
Minimal cache_trace runner
"""

import os
import sys
import subprocess

def run_cache_trace(target_args, pintool=None, pin_root=None):
    """Run cache_trace.so with given target arguments"""
    
    # Default paths
    if pintool is None:
        pintool = "tools/pintool/obj-intel64/cache_trace.so"
    
    if pin_root is None:
        pin_root = os.environ.get('PIN_ROOT', 'tools/sde-kit/pinkit')
    
    # Build command
    pin_cmd = os.path.join(pin_root, "pin")
    cmd = [pin_cmd, "-t", pintool, "--"] + target_args
    
    print(f"Running: {' '.join(cmd)}")
    
    # Execute
    subprocess.run(cmd)

if __name__ == "__main__":
    if len(sys.argv) < 2 or sys.argv[1] in ["-h", "--help"]:
        print(f"Usage: {sys.argv[0]} [target_program] [args...]")
        print(f"Example 1: {sys.argv[0]} /bin/ls -l")
        print(f"Example 2: {sys.argv[0]} -- /bin/ls -l")
        print("\nNote: The '--' separator is optional in this wrapper.")
        sys.exit(0)
    
    # Check if first argument is '--' and skip it
    if sys.argv[1] == "--":
        target_args = sys.argv[2:]
    else:
        target_args = sys.argv[1:]
    
    run_cache_trace(target_args)
