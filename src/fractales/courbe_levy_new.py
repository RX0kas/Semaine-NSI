# pour lancer ce fichier:
# Il faut être dans le dossier parent à src
# python -m src.fractales.courbe_levy_new
from src.application import Application
import src.cpp_backend as backend

def courbe_levy(profondeur : int, taille : int = 18000):
    t = backend.get_turtle()
    if profondeur == 0:
        t.forward(taille)
    else:
        t.right(45)
        courbe_levy(profondeur-1,taille/2)
        t.left(90)
        courbe_levy(profondeur-1,taille/2)
        t.right(45)


if __name__ == "__main__":
    r = Application()
    t = backend.get_turtle()

    t.up()
    t.goto(0,0)
    t.down()

    courbe_levy(15)

    r.run()