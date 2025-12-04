from src.cpp_backend import *

class Interface:
    selected_angle = 0

    def __init__(self):
        pass

    def update(self):
        pass

    def render(self):
        pass

    @staticmethod
    def placeFractale(x:int,y:int):
        selectedFractalesId = get_selected_fractales_id()
        # Setup turtle
        t = get_turtle()

        t.up()
        t.goto(x*100, y*100)
        t.angle = get_angle()
        t.down()

        setShowProgress(True)
        if selectedFractalesId=="arbre":
            from src.fractales.arbre import arbre
            arbre(get_profondeur(),get_longueur())
        elif selectedFractalesId=="arbre3":
            from src.fractales.arbre3 import arbre3
            arbre3(get_profondeur(),get_longueur())
        elif selectedFractalesId=="courbe_de_koch_quadratique":
            from src.fractales.courbe_de_koch_quadratique import courbe_de_koch_quadratique
            courbe_de_koch_quadratique(get_profondeur(),get_longueur())
        elif selectedFractalesId=="courbe_de_koch_quadratique_inv":
            from src.fractales.courbe_de_koch_quadratique_inv import courbe_de_koch_quadratique_inv
            courbe_de_koch_quadratique_inv(get_profondeur(),get_longueur())
        elif selectedFractalesId == "flocon_koch":
            from src.fractales.flocon_koch import flocon_koch
            flocon_koch(get_profondeur(),get_longueur())
        elif selectedFractalesId == "ligne_koch":
            from src.fractales.ligne_koch import ligne_koch
            ligne_koch(get_profondeur(),get_longueur())
        elif selectedFractalesId == "courbe_carre_sierpinski":
            from src.fractales.courbe_carre_de_sierpinski import courbe_carre_sierpinski
            courbe_carre_sierpinski(get_profondeur(),get_longueur())
        elif selectedFractalesId == "courbe_de_hilbert":
            from src.fractales.courbe_de_hilbert import courbe_de_hilbert
            courbe_de_hilbert(get_profondeur(),get_longueur())
        elif selectedFractalesId == "courbe_sierpinski":
            from src.fractales.courbe_de_sierpinski import courbe_sierpinski
            courbe_sierpinski(get_profondeur(),get_longueur())
        elif selectedFractalesId == "courbe_du_dragon":
            from src.fractales.courbe_du_dragon import courbe_du_dragon
            courbe_du_dragon(get_profondeur(),get_longueur())
        elif selectedFractalesId == "courbe_en_fleche_sierpinski":
            from src.fractales.courbe_en_fleche_de_sierpinski import courbe_en_fleche_sierpinski
            courbe_en_fleche_sierpinski(get_profondeur(),get_longueur())
        elif selectedFractalesId == "courbe_levy":
            from src.fractales.courbe_levy import courbe_levy
            courbe_levy(get_profondeur(),get_longueur())

        t.penup()
        t.pendown()
        setShowProgress(False)