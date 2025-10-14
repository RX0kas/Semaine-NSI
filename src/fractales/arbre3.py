import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(__file__)))) # dossier src

from src.application import Application
from src.returtle import Turtle


def arbre3(profondeur : int, taille : int = 110):
    t = Turtle.get_turtle()

    if profondeur == 0:
        for _ in range(3):
            t.forward(taille/2)
            t.left(180)
            t.forward(taille/2)
            t.left(180)
            t.left(120)

    else:
        for _ in range(3):
            t.forward(taille/2)
            arbre3(profondeur-1,taille/2)
            t.left(180)
            t.forward(taille/2)
            t.left(180)
            t.left(120)


if __name__ == "__main__":
    r = Application()
    t = Turtle.get_turtle()

    t.up()
    t.goto(0, -20)
    t.left(90)
    t.down()

    arbre3(11)

    r.run()