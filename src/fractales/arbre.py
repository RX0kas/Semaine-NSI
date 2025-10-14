import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(__file__)))) # dossier src

from src.application import Application
from src.returtle import Turtle


def arbre(profondeur : int, taille : int = 180):
    t = Turtle.get_turtle()

    t.forward(taille/3)

    if profondeur == 0:
        t.forward(taille/3)
        t.left(180)
        t.forward(taille/3)
        t.left(180)

    else:
        t.left(45)
        arbre(profondeur-1,2*taille/3)
        t.right(90)
        arbre(profondeur-1,2*taille/3)
        t.left(45)

    t.left(180)
    t.forward(taille/3)
    t.left(180)


if __name__ == "__main__":
    r = Application()
    t = Turtle.get_turtle()

    t.up()
    t.goto(0, -90)
    t.left(90)
    t.down()

    arbre(17)

    r.run()