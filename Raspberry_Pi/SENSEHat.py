while True:

  from sense_hat import SenseHat

  sense = SenseHat()
  sense.clear()

  temp = sense.get_temperature()
  print(temp)
  
  humidity = sense.get_humidity()
  humidity = round(humidity, 1)
  print(humidity)
