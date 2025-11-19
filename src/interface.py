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
        t.angle = Interface.selected_angle
        t.down()

        setShowProgress(True)
        if selectedFractalesId=="arbre":
            from src.fractales.arbre import arbre
            arbre(get_profondeur())
        elif selectedFractalesId=="arbre3":
            from src.fractales.arbre3 import arbre3
            arbre3(get_profondeur())
        elif selectedFractalesId=="courbe_de_koch_quadratique":
            from src.fractales.courbe_de_koch_quadratique import courbe_de_koch_quadratique
            courbe_de_koch_quadratique(get_profondeur())
        elif selectedFractalesId=="courbe_de_koch_quadratique_inv":
            from src.fractales.courbe_de_koch_quadratique_inv import courbe_de_koch_quadratique_inv
            courbe_de_koch_quadratique_inv(get_profondeur())
        elif selectedFractalesId == "flocon_koch":
            from src.fractales.flocon_koch import flocon_koch
            flocon_koch(get_profondeur())
        elif selectedFractalesId == "ligne_koch":
            from src.fractales.ligne_koch import ligne_koch
            ligne_koch(get_profondeur())
        setShowProgress(False)