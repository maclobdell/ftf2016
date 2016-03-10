/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __MBED_CLIENT_ETHERNET_CLIENT_H__
#define __MBED_CLIENT_ETHERNET_CLIENT_H__

#include <map>
#include <string>
#include <sstream>
#include <vector>
#include "minar/minar.h"
#include "wrapper.h"
#include "lwipv4_init.h"

using namespace mbed::util;
using namespace std;

EthernetInterface eth;

Serial &output = get_stdio_serial();

static MbedClient* client;
static map<string, M2MObject*> objects;
static map<string, M2MObjectInstance*> objectInstances;
static map<string, M2MResource*> resources;

struct MbedClientOptions mbed_client_get_default_options() {
    struct MbedClientOptions options;
    options.Manufacturer = "Manufacturer_String";
    options.Type = "Type_String";
    options.ModelNumber = "ModelNumber_String";
    options.SerialNumber = "SerialNumber_String";
    options.DeviceType = "test";
    options.SocketMode = M2MInterface::UDP;
    options.ServerAddress = "coap://api.connector.mbed.com:5684";

    return options;
}

static bool mbed_client_init() {
    output.printf("In mbed_client_init\r\n");
    // This sets up the network interface configuration which will be used
    // by LWM2M Client API to communicate with mbed Device server.
    eth.init();     //Use DHCP
    output.printf("Called eth.init()\r\n");
    if (eth.connect() != 0) {
        output.printf("[ETH] Failed to form a connection!\r\n");
        return false;
    }
    if (lwipv4_socket_init() != 0) {
        output.printf("[ETH] Error on lwipv4_socket_init!\r\n");
        return false;
    }
    output.printf("[ETH] IP address %s\r\n", eth.getIPAddress());
    output.printf("[ETH] Device name %s\r\n", MBED_ENDPOINT_NAME);

    // Create endpoint interface to manage register and unregister
    client->create_interface();

    // Create Objects of varying types, see simpleclient.h for more details on implementation.
    M2MSecurity* register_object = client->create_register_object(); // server object specifying connector info
    M2MDevice*   device_object   = client->create_device_object();   // device resources object

    // Create list of Objects to register
    M2MObjectList object_list;

    // Add objects to list
    object_list.push_back(device_object);

    map<string, M2MObject*>::iterator it;
    for (it = objects.begin(); it != objects.end(); it++)
    {
        object_list.push_back(it->second);
    }

    // Set endpoint registration object
    client->set_register_object(register_object);

    // Issue register command.
    FunctionPointer2<void, M2MSecurity*, M2MObjectList> fp(client, &MbedClient::test_register);
    minar::Scheduler::postCallback(fp.bind(register_object, object_list));
    minar::Scheduler::postCallback(client, &MbedClient::test_update_register).period(minar::milliseconds(25000));

    return true;
}

bool mbed_client_setup() {
    output.printf("In mbed_client_setup\r\n");
    if (client) {
        output.printf("[ETH] [ERROR] mbed_client_setup called, but mbed_client is already instantiated\r\n");
        return false;
    }

    struct MbedClientOptions options = mbed_client_get_default_options();

    client = new MbedClient(options);
    return mbed_client_init();
}

bool mbed_client_setup(MbedClientOptions options) {
    if (client) {
        output.printf("[ETH] [ERROR] mbed_client_setup called, but mbed_client is already instantiated\r\n");
        return false;
    }
    client = new MbedClient(options);
    return mbed_client_init();
}

void mbed_client_on_registered(void(*fn)(void)) {
    FunctionPointer0<void> fp(fn);
    client->set_registered_function(fp);
}

void mbed_client_on_unregistered(void(*fn)(void)) {
    FunctionPointer0<void> fp(fn);
    client->set_unregistered_function(fp);
}

static vector<string> parse_route(const char* route) {
    string s(route);
    vector<string> v;
    stringstream ss(s);
    string item;
    while (getline(ss, item, '/')) {
        v.push_back(item);
    }
    return v;
}

bool mbed_client_define_resource(const char* route, std::string v, M2MBase::Operation opr, bool observable) {
    if (client) {
        output.printf("[ETH] [ERROR] mbed_client_define_resource, Can only define resources before mbed_client_setup is called!\r\n");
        return false;
    }

    auto segments = parse_route(route);
    if (segments.size() != 2) {
        output.printf("[ETH] [ERROR] mbed_client_define_resource, Route needs to have two segments, split by '/' (%s)\r\n", route);
        return false;
    }

    M2MObjectInstance* inst;
    if (objectInstances.count(segments.at(0))) {
        output.printf("Found object... %s\r\n", segments.at(0).c_str());
        inst = objectInstances[segments.at(0)];
    }
    else {
        output.printf("Create new object... %s\r\n", segments.at(0).c_str());
        auto obj = M2MInterfaceFactory::create_object(segments.at(0).c_str());
        inst = obj->create_object_instance();
        objects.insert(std::pair<string, M2MObject*>(segments.at(0), obj));
        objectInstances.insert(std::pair<string, M2MObjectInstance*>(segments.at(0), inst));
    }

    // @todo check if the resource exists yet
    M2MResource* res = inst->create_dynamic_resource(segments.at(1).c_str(), "",
        M2MResourceInstance::STRING, observable);
    res->set_operation(opr);
    res->set_value((uint8_t*)v.c_str(), v.length());

    string route_str(route);
    resources.insert(std::pair<string, M2MResource*>(route_str, res));

    return true;
}

bool mbed_client_define_resource(const char* route, int v, M2MBase::Operation opr, bool observable) {
    stringstream ss;
    ss << v;
    std::string stringified = ss.str();
    return mbed_client_define_resource(route, stringified, opr, observable);
}

bool mbed_client_set(const char* route, string v) {
    string route_str(route);
    if (!resources.count(route_str)) {
        output.printf("[ETH] [ERROR] No such route (%s)\r\n", route);
        return false;
    }
    resources[route_str]->set_value((uint8_t*)v.c_str(), v.length());
    return true;
}

bool mbed_client_set(const char* route, int v) {
    stringstream ss;
    ss << v;
    std::string stringified = ss.str();

    return mbed_client_set(route, stringified);
}

string mbed_client_get(const char* route) {
    string route_str(route);
    if (!resources.count(route_str)) {
        output.printf("[ETH] [ERROR] No such route (%s)\r\n", route);
        return string();
    }

    uint8_t* buffIn;
    uint32_t sizeIn;
    resources[route_str]->get_value(buffIn, sizeIn);

    std::string s((char*)buffIn, sizeIn);
    return s;
}

int mbed_client_get_int(const char* route) {
    string v = mbed_client_get(route);
    if (v.empty()) return 0;

    return atoi((const char*)v.c_str());
}

bool mbed_client_define_function(const char* route, void(*fn)(void*)) {
    if (!mbed_client_define_resource(route, string(), M2MBase::POST_ALLOWED, false)) {
        return false;
    }

    string route_str(route);
    if (!resources.count(route_str)) {
        output.printf("[ETH] [ERROR] Should be created, but no such route (%s)\r\n", route);
        return false;
    }

    FunctionPointer1<void, void*>* fp = new FunctionPointer1<void, void*>(fn);
    resources[route_str]->set_execute_function(execute_callback(fp, &FunctionPointer1<void, void*>::call));
    return true;
}

bool mbed_client_define_function(const char* route, execute_callback fn) {
    if (!mbed_client_define_resource(route, string(), M2MBase::POST_ALLOWED, false)) {
        return false;
    }

    string route_str(route);
    if (!resources.count(route_str)) {
        output.printf("[ETH] [ERROR] Should be created, but no such route (%s)\r\n", route);
        return false;
    }
    // No clue why this is not working?! It works with class member, but not with static function...
    resources[route_str]->set_execute_function(fn);
    return true;
}

#endif // __MBED_CLIENT_ETHERNET_CLIENT_H__
