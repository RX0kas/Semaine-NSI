# pour lancer ce fichier:
# Il faut être dans le dossier parent à src
# python -m src.fractales.flocon_koch

from ..application import Application # on peut utiliser ces deux methodes
from src.fractales.ligne_koch import ligne_koch
import src.cpp_backend as backend

def flocon_koch(profondeur : int,taille:int = 140):
    t = backend.get_turtle()
    t.left(180)
    ligne_koch(profondeur,taille)
    t.right(120)
    ligne_koch(profondeur,taille)
    t.right(120)
    ligne_koch(profondeur,taille)


if __name__ == "__main__":
    r = Application()
    t = backend.get_turtle()

    t.penup()
    t.goto(70, -50)
    t.pendown()

    flocon_koch(5)

    r.run()