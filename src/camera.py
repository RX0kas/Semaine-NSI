import warnings

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
        self.__compute_view_matrix()
        
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
        self.__view_matrix = np.matrix([[self.__zoom,0,          (1-self.__zoom)*self.__x],
                                            [0,           self.__zoom,(1-self.__zoom)*self.__y],
                                            [0,           0          ,1
                                             ]],dtype=np.float32)
    
    def __get_view_matrix(self):
        if self.__view_matrix is None:
            warnings.warn("View matrix not computed")
            self.__compute_view_matrix()
        return self.__view_matrix

    def __set_x(self, x):
        self.__x = x
        self.__compute_view_matrix()

    def __get_x(self):
        return self.__x

    def __set_y(self, y):
        self.__y = y
        self.__compute_view_matrix()

    def __get_y(self):
        return self.__y

    def __set_zoom(self, zoom):
        self.__zoom = zoom
        self.__compute_view_matrix()

    def __get_zoom(self):
        return self.__zoom

    x = property(__get_x, __set_x)
    y = property(__get_y, __set_y)
    zoom = property(__get_zoom, __set_zoom)
    view_matrix = property(__get_view_matrix)