      //Power down between sends to save power, measured in seconds.
      //System.sleep(SLEEP_MODE_DEEP,25);  //for Particle Photon
      //Spark.sleep(SLEEP_MODE_DEEP,300);   //for Spark Core


      WiFi.off();

      delay(25000);

      WiFi.connect();
