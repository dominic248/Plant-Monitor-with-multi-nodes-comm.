Tech-Stack: NodeMCU-ESP8266, OLED, Moisture sensor, Temp sensor. 
- Multiple wireless nodes communicate via NodeMCU through a main node which is a NodeMCU which acts as an Access Point without Internet Connection. 
- All nodes share data (moisture and temperature) with the main node, the main node reads the data and checks the condition for moisture and temperature of the soil and sends responses back to the respective nodes to start the pump or not for that specific node. 
- Hence, conditions for all nodes can be controlled from the main NodeMCU. 
- The nodes work without Internet connection.
