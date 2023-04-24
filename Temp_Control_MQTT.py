# Temperature Control Publish/Subscribe Through MQTT
# Authors: Conrad Obeng, Matthew Moubray, Ethan Nguy
# Description: This code subscribes to an MQTT Topic that receives a temperature value from a sensor.
# This value is then published to a separate MQTT Topic, to be received by a different client.

import paho.mqtt.client as mqtt # Paho library for MQTT Communication

# MQTT Subscribe function
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("esp/ds18b20/temperature") # Client subscribed to Topic

# MQTT Message Received function
def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload)) # Print Message Topic and Payload
    temp_value = float(msg.payload)
    temp_comparison(temp_value) # Call Temperature Comparison function

# Temperature Publish function
def temp_comparison(temp_value):
    # If the temperature value is under 32 degrees Fahrenheit, the client publishes a 1. Otherwise, a 0 is published.
    if temp_value < 32: 
        client.publish("esp32/OutputControl", payload=1)
    else:
        client.publish("esp32/OutputControl", payload=0)

# MQTT Connection function
def mqtt_connection():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    # Client connected on localhost, port 1883, with a 60 second maximum period allowed between communication with the broker.
    client.connect("localhost", 1883, 60)

    client.loop_forever()

if __name__ == "__main__":
    mqtt_connection()
