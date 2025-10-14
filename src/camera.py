import numpy as np

class Camera:
    """
    Pour l'instance contient juste les données comme la position et le zoom
    Car la logique est ecrite sur le shader
    """
    def __init__(self):
        self.__x = 0
        self.__y = 0
        self.__zoom = 0.5
        self.__view_matrix = None
        
    def __compute_view_matrix(self):
        """
        s: Facteur du zoom
        X' & Y': Position projeter
        Xc & Yc: Centre du zoom
        X & Y: Position actuel (Multiplication effectuer dans le Shader)
        
        [X']   [s 0 (1-s)Xc]   [x]
        [Y'] = [0 s (1-s)Yc] * [y]
        [1]    [0 0       1]   [1]
        """
        self.__view_matrix = np.matrix([[self.__zoom,0,(1-self.__zoom)*self.__x],
                                        [0,self.__zoom,(1-self.__zoom)*self.__x],
                                        [0,0,]],dtype=np.float32)
    
    def get_view_matrix(self):
        return self.__view_matrix

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
    view_matrix = property(get_view_matrix)