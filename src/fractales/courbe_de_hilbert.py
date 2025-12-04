# pour lancer ce fichier:
# Il faut être dans le dossier parent à src
# python -m src.fractales.courbe_de_hilbert_new
from src.application import Application
import src.cpp_backend as backend

def courbe_de_hilbert(profondeur : int, taille : int = 180):
    t = backend.get_turtle()
    av = taille/(2**profondeur)
    courbe_de_hilbert_récur1(profondeur,av)
    


def courbe_de_hilbert_récur1(profondeur : int, taille):
    t = backend.get_turtle()
    if profondeur == 0:
        pass
    else:
        t.right(90)
        courbe_de_hilbert_récur2(profondeur-1,taille)
        t.forward(taille)
        t.left(90)
        courbe_de_hilbert_récur1(profondeur-1,taille)
        t.forward(taille)
        courbe_de_hilbert_récur1(profondeur-1,taille)
        t.left(90)
        t.forward(taille)
        courbe_de_hilbert_récur2(profondeur-1,taille)
        t.right(90)




def courbe_de_hilbert_récur2(profondeur : int, taille):
    t = backend.get_turtle()
    if profondeur == 0:
        pass
    else:
        t.left(90)
        courbe_de_hilbert_récur1(profondeur-1,taille)
        t.forward(taille)
        t.right(90)
        courbe_de_hilbert_récur2(profondeur-1,taille)
        t.forward(taille)
        courbe_de_hilbert_récur2(profondeur-1,taille)
        t.right(90)
        t.forward(taille)
        courbe_de_hilbert_récur1(profondeur-1,taille)
        t.left(90)
        



if __name__ == "__main__":
    r = Application()
    t = backend.get_turtle()

    t.up()
    t.goto(0, 0)
    t.down()

    courbe_de_hilbert(7)

    r.run()