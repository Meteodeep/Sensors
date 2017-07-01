import webbrowser
from sense_hat import SenseHat

sense = SenseHat()

while True:

        acceleration = sense.get_accelerometer_raw()
        x = acceleration['x']
        y = acceleration['y']
        z = acceleration['z']

        x=round(x, 0)
        y=round(y, 0)
        z=round(z, 0)

        if x != 0 and y != 0 or z != 1:
                f = open('/var/www/meteodeep/earthquake.html','w')

                message = """<html>
                <head></head>
                <body><h1>Earthquake</h1><br><iframe src="https://giphy.com/embed/UmPl68cxazEgo" width="480" height="120" frameBorder="0" class="giphy-embed" allowFullScreen></iframe><p><a href="https://giphy.com/gifs$
                </html>"""

                f.write(message)
                f.close()


        print("x={0}, y={1}, z={2}".format(x, y, z))
