#ifndef MQTT_CONFIG_H
#define MQTT_CONFIG_H

// --- Broker Configuration ---
#define MQTT_BROKER_IP   "192.168.1.178"
#define MQTT_PORT        1883
#define MQTT_CLIENT_ID   "zarzara_node_d3"

// --- Classroom Configuration ---
#define CLASSROOM_NAME   "d3"

// --- Topic Templates ---
// smartclass/classrooms/<name>/...
#define TOPIC_BASE             "smartclass/classrooms/" CLASSROOM_NAME
#define TOPIC_EVENTS           TOPIC_BASE "/events"
#define TOPIC_COMMANDS         TOPIC_BASE "/commands"
#define TOPIC_ACCESS_REQUEST   TOPIC_BASE "/access/request"
#define TOPIC_ACCESS_RESPONSE  TOPIC_BASE "/access/response"
#define TOPIC_ATTEND_REQUEST   TOPIC_BASE "/attendance/request"
#define TOPIC_DELAY_REQUEST    TOPIC_BASE "/door-delay/request"
#define TOPIC_DELAY_RESPONSE   TOPIC_BASE "/door-delay/response"

#endif
