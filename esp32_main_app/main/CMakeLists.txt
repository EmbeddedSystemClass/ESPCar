set(COMPONENT_SRCS  "main.c" 
                    "sensors/gps.c" 
                    "sensors/ultrasonic.c" 
                    "sensors/accelmagneto.c" 
                    "comms/tcpconn.c"
                    "actuators/motorcontrol.c")

set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()