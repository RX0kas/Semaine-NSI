from enum import Enum

class ShaderType(Enum):
    VERTEX = 1
    FRAGMENT = 2
    LINKING = 3

class ShaderError(Exception):
    def __init__(self, type, message="Impossible de compiler le shader"):
        ERRmessage = "ERROR "
        if type == ShaderType.VERTEX:
            ERRmessage += "VERTEX"
        elif type == ShaderType.FRAGMENT:
            ERRmessage += "FRAGMENT"
        elif type == ShaderType.LINKING:
            ERRmessage += "LINKING"

        super().__init__(f"{ERRmessage}: {message}")