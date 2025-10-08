
class Camera:
    """
    Pour l'instance contient juste les données comme la position et le zoom
    Car la logique est ecrite sur le shader
    """
    def __init__(self):
        self.__x = 0
        self.__y = 0
        self.__zoom = 1

    def set_x(self,x):
        self.__x = x

    def get_x(self):
        return self.__x

    def set_y(self,y):
        self.__y = y

    def get_y(self):
        return self.__y

    def set_zoom(self,zoom):
        self.__zoom = zoom

    def get_zoom(self):
        return self.__zoom

    x = property(get_x, set_x)
    y = property(get_y, set_y)
    zoom = property(get_zoom, set_zoom)