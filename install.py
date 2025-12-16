import os,sys

SCRIPT_DIR = os.path.dirname(__file__)
VENV_DIR = os.path.join(SCRIPT_DIR,".venv")

def activer_venv():
    if not os.path.exists(VENV_DIR):
        os.system("python -m venv "+VENV_DIR)
    
    os.system(os.path.join(SCRIPT_DIR,".venv","Scripts","activate"))
    
def installer_dependance():
    os.system("")

activer_venv()
input()