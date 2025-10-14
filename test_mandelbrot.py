import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(__file__)))) # dossier src

from src.application import Application
from src.shader import Shader


if __name__ == "__main__":
    r = Application()
    s = Shader("mandelbrot")
    s.use()
    
    r.run()