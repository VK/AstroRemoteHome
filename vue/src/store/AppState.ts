import { Module, VuexModule, Mutation, Action, } from "vuex-module-decorators";
import { Client } from "paho-mqtt";
import store from '@/store';
import "../types";
import LZString from "lz-string";
import { VMenu } from 'vuetify/lib';



@Module({ namespaced: true })
export default class AppState extends VuexModule {


    public storeMqttConfig: boolean = true;
    public mqttClientID: string = "";
    public mqttServer: string = "";
    public mqttUser: string = "";
    public mqttPassword: string = "";
    public mqttPort: number = 8883;

    public pin: string = "1543";

    public connectionState = "Offline";
    public client: any;

    public Esps: { [id: string]: any; } = {};
    public NetworkDevices: { [mac: string]: { [ip: string]: SingleDevice; } } = {};


    public static pad(value: any, size: any = 2): String {
        var s = String(value);
        while (s.length < size) { s = "0" + s; }
        return s;
    }

    @Mutation
    public enableSaveMQTTconfig(): void {
        this.storeMqttConfig = true;
    }

    @Mutation
    public updateMQTTconfig(newConfig: any): void {

        //create a new client id if needed
        if (newConfig.clientId && newConfig.clientId !== "") {
            this.mqttClientID = newConfig.clientId;
        } else {
            if (this.mqttClientID && this.mqttClientID !== "") {
                newConfig.clientId = this.mqttClientID;
            } else {
                newConfig.clientId = "webApp_" + String(Math.round(Math.random() * 100000));
                this.mqttClientID = newConfig.clientId;
            }
        }

        // save the data the state
        this.mqttServer = newConfig.server;
        this.mqttUser = newConfig.user;
        this.mqttPassword = newConfig.password;
        this.mqttPort = Number(newConfig.port);

        // save the data in local storage
        if (this.storeMqttConfig) {
            localStorage.setItem("mqttConfig", JSON.stringify(newConfig));
            console.log("save local config");
            var encoded = LZString.compressToEncodedURIComponent(JSON.stringify(newConfig));
            console.log(encoded);
        }

        this.connectionState = "Connecting";
        store.commit("appState/connectMQTT");
    }

    @Mutation
    public initMQTTconfig(): void {
        let uri = window.location.href.split('?');
        //try to parse for settings
        if (uri.length == 2) {
            try {
                this.storeMqttConfig = false;
                let newConfig: any = JSON.parse(LZString.decompressFromEncodedURIComponent(uri[1]));
                store.commit("appState/updateMQTTconfig", newConfig);
            } catch {
                console.log("Problems parsing the settings");
            }
            return

        }

        if (localStorage.getItem("mqttConfig")) {
            let value: string = localStorage.getItem("mqttConfig") as string;
            let newConfig: any = JSON.parse(value);

            store.commit("appState/updateMQTTconfig", newConfig);
        }
    }

    @Mutation
    public connectMQTT(): void {

        store.commit("socketConfig/reset");
        this.Esps = {};


        this.client = new Client(this.mqttServer, this.mqttPort, this.mqttClientID);

        // set callback handlers
        this.client.onConnectionLost = (responseObject: any) => {
            if (responseObject.errorCode !== 0) {
                store.commit("appState/onMQTTconnectionLost", responseObject.errorMessage);
            }
        };
        this.client.onMessageArrived = (message: any) => {
            store.commit("appState/onMQTTmessage", message);
        };
        var options: any = {
            useSSL: true,
            userName: this.mqttUser,
            password: this.mqttPassword,
            onSuccess: () => {
                store.commit("appState/onMQTTconnected");
            },
            onFailure: (error: any) => {
                store.commit("appState/onMQTTfailure", error);
            }
        };

        // connect the client
        this.client.connect(options);
    }

    @Mutation
    public disconnectMQTT(): void {
        this.connectionState = "Offline";
        this.client.disconnect();
    }


    @Mutation
    public onMQTTconnected(): void {
        this.client.subscribe("MySockets/#");
        this.client.subscribe("Esp/#");
        this.connectionState = "Connected";
    }

    @Mutation
    public onMQTTfailure(error: any): void {
        this.connectionState = "Connection Error";
        console.log(error);
    }

    @Mutation
    public onMQTTconnectionLost(error: any): void {
        this.connectionState = "Connection Lost";
        console.log(error);
    }

    @Mutation
    public onMQTTmessage(message: any): void {


        if (message.topic) {
            let topic: string = message.topic;


            //handle esp registration
            if (topic.startsWith("Esp")) {
                let parts = topic.split('/');


                if (parts.length == 3) {
                    if (!(parts[1] in this.Esps)) {
                        this.Esps[parts[1]] = {

                        };
                    }

                    if (parts[2] == "IP") {
                        this.Esps[parts[1]]["IP"] = message.payloadString;
                        this.Esps[parts[1]]["UP"] = new Date().toLocaleTimeString();
                    }

                    if (parts[2] == "LOC") {
                        let data = JSON.parse(message.payloadString);
                        this.Esps[parts[1]]["LONG"] = data["longitude"];
                        this.Esps[parts[1]]["LAT"] = data["latitude"];
                    }

                    if (parts[2] == "MasterState") {

                        if (message.payloadString == "online") {
                            this.Esps[parts[1]]["MODE"] = "at home";
                        } else {
                            this.Esps[parts[1]]["MODE"] = "away";
                        }

                    }

                    if (parts[2] == "VERS") {
                        this.Esps[parts[1]]["VERS"] = message.payloadString;
                    }

                    if (parts[2] == "SUN") {
                        let data = JSON.parse(message.payloadString);

                        this.Esps[parts[1]]["RISE"] =
                            String(Math.floor(data["sunrise"] / 60)) + ":"
                            + AppState.pad(data["sunrise"] % 60);
                        this.Esps[parts[1]]["SET"] = String(Math.floor(data["sunset"] / 60)) + ":"
                            + AppState.pad(data["sunset"] % 60);
                        this.Esps[parts[1]]["UP"] = data["now"];
                    }
                }


                //handle 4 component network devices in the area of certain Esps
                if (parts.length == 4) {
                    if (message.payloadString !== "") {

                        if (!(parts[1] in this.NetworkDevices)) {
                            this.NetworkDevices[parts[1]] = {}
                        }

                        this.NetworkDevices[parts[1]][parts[3]] = {
                            state: message.payloadString
                        }

                    } else {
                        delete this.NetworkDevices[parts[1]][parts[3]];
                    }
                   
                }
            }

            if (topic.startsWith("MySockets/")) {
                if (message.payloadString && message.payloadString.length > 0) {
                    if (message.payloadString != "OFF" && message.payloadString != "ON") {
                        store.commit("socketConfig/importConfig", message.payloadString);
                    }
                } else {
                    store.commit("socketConfig/deleteSocketSpecial", message.topic.replace("MySockets/", ""));
                }
            }


            if (topic.endsWith("/received")) {
                if (this.receivedRadioConfigArray.length >= 3) {
                    this.receivedRadioConfigArray.shift();
                }
                this.receivedRadioConfigArray.push(message.payloadString);


                if (this.receivedRadioConfigArray.length === 3) {
                    let set: any = new Set(this.receivedRadioConfigArray);
                    if (set.size === 1) {
                        let data: any = JSON.parse(this.receivedRadioConfigArray[0]);
                        this.receivedRadioConfig = {
                            protocol: data[0],
                            length: data[1],
                            value: data[2]

                        };
                    }
                }

            }
        }

        //console.log(message);
    }



    /* handle device add and removal */

    @Mutation
    public addNetworkDevice(message: any): void {
        this.client.publish("Esp/" + message.mac + "/Master/" + message.ip, "new", 2, true);
    }

    @Mutation
    public removeNetworkDevice(message: any): void {
        this.client.publish("Esp/" + message.mac + "/Master/" + message.ip, "", 2, true);
    }

    /* end handle device add and removal */




    /* handle radio signal detection */
    public receivedRadioConfigArray: string[] = [];
    public receivedRadioConfig: RadioSignal | null = null;

    @Mutation
    public resetReceivedListen(): void {
        this.receivedRadioConfigArray = [];
        this.receivedRadioConfig = null;
    }
    /* end handle radio signal detection */





}