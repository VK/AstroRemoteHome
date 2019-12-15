import { Module, VuexModule, Mutation } from "vuex-module-decorators";
import "../types";
import store from '@/store';



@Module({ namespaced: true })
export default class MySocketsConfig extends VuexModule {

    public config: AllConfigs = [];


    @Mutation
    public reset(data: any): void {
       this.config = [];
    }

    @Mutation
    public addNewSocket(data: any): void {
        this.config.push(data);
        store.state.appState.client.publish("MySockets/" + data.name, JSON.stringify(data), 2, true);
    }

    @Mutation
    public addNewSwitch(data: SingleConfig): void {
        let days: string[] = [];
        let newAutoEntry: AutomaticSwitching = {
            state: true,
            start: "",
            end: "",
            days: days,
            sD: 0,
            eD: 0,
            sT: "t",
            eT: "t"
        };

        this.config.forEach((el: SingleConfig, idx) => {
            if (el.name === data.name) {
                el.auto.push(newAutoEntry);
                store.state.appState.client.publish("MySockets/" + el.name, JSON.stringify(el), 2, true);
            }
        });
    }

    @Mutation
    public deleteSwitch(input: { singleConfig: SingleConfig, arrayIndex: number }): void {
        if (input.arrayIndex !== undefined) {
            this.config.forEach((el: SingleConfig, idx) => {
                if (el.name === input.singleConfig.name) {
                    el.auto.splice(input.arrayIndex, 1);

                    store.state.appState.client.publish("MySockets/" + el.name, JSON.stringify(el), 2, true);
                }
            });
        }
    }


    @Mutation
    public deleteSocket(singleConfig: SingleConfig): void {

        let delIndex: number = -10;
        this.config.forEach((el: SingleConfig, idx) => {
            if (el.name === singleConfig.name) {
                delIndex = idx;
            }
        });

        if (delIndex !== -10) {
            this.config.splice(delIndex, 1);
            store.state.appState.client.publish("MySockets/" + singleConfig.name, "", 2, true);
        }

    }

    // delete a socket if the socket was deleted by somebody else on the mqtt server
    @Mutation
    public deleteSocketSpecial(name: string): void {
        let delIndex: number = -10;
        this.config.forEach((el: SingleConfig, idx) => {
            if (el.name === name) {
                delIndex = idx;
            }
        });

        if (delIndex !== -10) {
            this.config.splice(delIndex, 1);
        }
    }

    @Mutation
    public saveSocket(singleConfig: SingleConfig): void {
        let delIndex: number = -10;
        this.config.forEach((el: SingleConfig, idx) => {
            if (el.name === singleConfig.name) {
                store.state.appState.client.publish("MySockets/" + el.name, JSON.stringify(el), 2, true);
            }
        });
    }




    @Mutation
    public importConfig(data: string): void {

        let newData: SingleConfig = JSON.parse(data);

        let found: boolean = false;
        this.config.forEach((el: SingleConfig, idx) => {
            if (el.name === newData.name) {
                found = true;

                el.auto = newData.auto;
                el.off = newData.off;
                el.on = newData.on;
                el.state = newData.state;
            }
        });
        if (!found) {
            this.config.push(newData);
        }
    }

    @Mutation
    public toggleSwitch(data: any): void {

        let found: SingleConfig | null = null;
        this.config.forEach((el: SingleConfig) => {
            if (el.name == data.name) {
                found = el;
                el.state = data.value;
            }
        });
        if (found)
            store.state.appState.client.publish("MySockets/" + data.name, JSON.stringify(found), 2, true);
    }





}