# pour lancer ce fichier:
# Il faut être dans le dossier parent à src
# python -m src.fractales.courbe_de_sierpinski_new
from src.application import Application
import src.cpp_backend as backend

def courbe_sierpinski(profondeur : int, taille : int = 1800):
    t = backend.get_turtle()
    av=taille/(profondeur+1)
    t.forward(av)
    t.right(90)
    courbe_sierpinski_récur(profondeur,av)
    t.forward(av)
    t.right(90)
    t.forward(av)
    t.right(90)
    courbe_sierpinski_récur(profondeur,av)
    t.forward(av)

def courbe_sierpinski_récur(profondeur : int, taille):
    t = backend.get_turtle()
    if profondeur==0:
        pass
    else:
        courbe_sierpinski_récur(profondeur-1,taille)
        t.forward(taille)
        t.left(45)
        t.forward(taille/2)
        t.left(45)
        courbe_sierpinski_récur(profondeur-1,taille)
        t.forward(taille)
        t.right(90)
        t.forward(taille)
        t.right(90)
        courbe_sierpinski_récur(profondeur-1,taille)
        t.forward(taille)
        t.left(45)
        t.forward(taille/2)
        t.left(45)
        courbe_sierpinski_récur(profondeur-1,taille)

    



if __name__ == "__main__":
    r = Application()
    t = backend.get_turtle()

    t.up()
    t.goto(0, 0)
    t.down()

    courbe_sierpinski(7)

    r.run()