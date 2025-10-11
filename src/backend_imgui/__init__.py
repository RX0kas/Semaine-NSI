from importlib import import_module
import os
import sys

# Localisation du module compilé (.pyd)
_module_name = "imgui_py_backend"
_module_path = os.path.dirname(__file__)

# Ajoute le dossier build/Release au sys.path pour l'import
if _module_path not in sys.path:
    sys.path.insert(0, _module_path)

# Import du module compilé
_imgui = import_module(_module_name)

# Réexporte tout le contenu vers le namespace courant
globals().update({k: getattr(_imgui, k) for k in dir(_imgui) if not k.startswith("_")})

# Nettoyage
del _imgui, _module_name, _module_path, import_module, os, sys