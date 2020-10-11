import pathlib
import random

archivo = ""
elementos = []
users = ["Hugo", "Juana", "Luis", "Paco", "Isabel"]
servidor = 0

def ingresar_archivo():
    archivo = str(input("Ingrese nombre del archivo: "))
    file = open(archivo)
    content = file.read()
    file.close()
    
    oraciones = content.split(",")

    i = 0
    for i in oraciones:
        tmp_user = users[random.randint(0,2)]
        obj = {}
        obj[tmp_user] = i
        elementos.append(obj)

    print(elementos[0])

def ingresar_servidor():
    servidor = str(input("Ingrese dirección IP del servidor: "))
    print("El servidor es: " + servidor)

def ver_datos():
    for i in elementos:
        print(i)

def enviar_datos():
    try:
        print("posting to server...")
        for i in elementos:
            print(json.dumps(i))
            r = requests.post(server + "/document", data = json.dumps(i))
            if r.status_code == requests.codes.ok:
                print(r.text)
                print("Server " + server + " posting....")
                return r.text
            else:
                print("error posting to server " + server)
                print(str(r.status_code))
                app.logger.info("Error posting document to '%s'.", server)
                return None
    except Exception as e:
        app.logger.info("Error posting document to '%s'.", server)
        app.logger.info()
        return None



def menu():
    print("1. Ingresar ruta de archivo")
    print("2. Ingresar dirección servidor")
    print("3. Ver datos")
    print("4. Enviar datos")
    print("0. Salir")


menu()
option = int(input("Ingrese una opción: "))

while option != 0:
    if option == 1:
        ingresar_archivo()
    elif option == 2:
        ingresar_servidor()
    elif option == 3:
        ver_datos()
    elif option == 4:
        enviar_datos()
    else:
        print("Opción no permitida.")

    print()
    menu()
    option = int(input("Ingrese una opción: "))

print("Gracias por usar el programa.")


    