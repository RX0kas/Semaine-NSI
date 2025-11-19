# pour lancer ce fichier:
# Il faut être dans le dossier parent à src
# python -m src.main


from src.application import Application
from src.fractales.ligne_koch import ligne_koch
import src.cpp_backend as backend

if __name__ == "__main__":
    r = Application()

    r.run()