import os
from pathlib import Path

# Get absolute paths
_current_dir = Path(__file__).parent
_bin_dir = _current_dir / "bin"

# Add bin directory to DLL search path
if hasattr(os, 'add_dll_directory'):
    os.add_dll_directory(str(_bin_dir))

# Also add to PATH for compatibility
os.environ['PATH'] = str(_bin_dir) + os.pathsep + os.environ['PATH']

# Now import the .pyd module normally
from .imgui_py_backend import *


print("imgui_py_backend loaded successfully")

# Cleanup
del _current_dir, _bin_dir