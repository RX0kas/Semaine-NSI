from src.application import Application
from src.returtle import Turtle

def ligne_koch(profondeur : int, taille : int = 180):
    t = Turtle.get_turtle()

    if profondeur==0:
        t.forward(taille)
    else:
        ligne_koch(profondeur-1, taille/3)
        t.left(60)
        ligne_koch(profondeur-1, taille/3)
        t.right(120)
        ligne_koch(profondeur-1, taille/3)
        t.left(60)
        ligne_koch(profondeur-1, taille/3)


if __name__ == "__main__":
    r = Application()
    t = Turtle.get_turtle()

    t.up()
    t.goto(-90, -90)
    t.down()

    ligne_koch(5)

    r.run()