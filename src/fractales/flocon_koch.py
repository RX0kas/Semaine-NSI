from src.application import Application
from src.returtle import Turtle
from ligne_koch import ligne_koch

def flocon_koch(profondeur : int):
    t = Turtle.get_turtle()

    t.left(180)
    ligne_koch(profondeur,140)
    t.right(120)
    ligne_koch(profondeur,140)
    t.right(120)
    ligne_koch(profondeur,140)


if __name__ == "__main__":
    r = Application()
    t = Turtle.get_turtle()

    t.up()
    t.goto(70, -50)
    t.down()

    flocon_koch(7)

    r.run()