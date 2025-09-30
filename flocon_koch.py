import turtle

s = turtle.getscreen()
t = turtle.Turtle()

def flocon_koch(profondeur : int):
    if profondeur==0:
        t.forward(30)
    else:
        flocon_koch(profondeur-1)
        t.left(45)
        flocon_koch(profondeur-1)
        t.right(90)
        flocon_koch(profondeur-1)
        t.left(45)
        flocon_koch(profondeur-1)

t.up()
t.goto(-300,-300)
t.down()
flocon_koch(2)
s.mainloop()