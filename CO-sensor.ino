// Adapted from https://www.hackster.io/leela-flynn-17/home-improvement-diy-wifi-gas-detector-with-text-alerts-49073d

// selects the u.FL antenna, comment out for the onboard (chip) antenna
STARTUP(WiFi.selectAntenna(ANT_EXTERNAL)); 

//Initialize timer to 0
int AlertSentSecsAgo = 0;
//This is used to publish Particle events
char strTxt[40];
//Define the PIN for the gas sensor
int GasSensor = A0;
//Define how often to send text alerts
int AlertFrequency = 30;
//Set sensor to "warm up" before we take readings
boolean initFlag = true;

void setup() 
{
     pinMode(GasSensor, INPUT);
     Serial.begin(9600);
     delay(5000); // wait so I can connect the USB serial monitor after power up
}

void loop()
{
    if (initFlag == true){
        Particle.publish("sensor status","warming up",PRIVATE);
        Serial.println("Just published sensor is warming up");
        // and now we wait for the sensor to warm up.... for 2 freakin days!! 
        delay(172800000);  // this what the default warm up time should be... 2 DAYS for the MQ-9 sensor
        Particle.publish("sensor status","normal",PRIVATE);
        Serial.println("Just published sensor is normal and done warming up");
        initFlag = false;
    }

     //I had to fine tune this value at which the alert is triggered based on some trial tests
     if (analogRead(GasSensor) > 750)
     {
          //If we sent an alert more than 30 seconds ago, send another one
          if (AlertSentSecsAgo >= AlertFrequency)
          {
               //Send alert every 30 seconds
               AlertSentSecsAgo = 0;
               sprintf(strTxt, "%u", analogRead(GasSensor));
               //Publish an event to the spark dashboard with the current value of the sensor
               Particle.publish("Gas Sensor Triggered", strTxt,PRIVATE); 
               Serial.println("Gas sensor triggered");
          }
        delay(1000);
        AlertSentSecsAgo++;
     }
     else
     {
          sprintf(strTxt, "%u", analogRead(GasSensor));
          Particle.publish("Gas Sensor normal", strTxt,PRIVATE);
          //Make sure that the alert will be sent next time the sensor is triggered
          AlertSentSecsAgo = AlertFrequency + 1; 
          delay(60000); //Wait 60 seconds before checking the sensor again
     }
}
