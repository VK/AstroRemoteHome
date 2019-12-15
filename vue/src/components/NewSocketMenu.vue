<template>
  <v-dialog v-model="dialog" persistent max-width="600px">
    <v-card>
      <v-card-title>
        <span class="headline">{{$t('word.newSocket')}}</span>
      </v-card-title>
      <v-card-text>
        <v-container>
          <v-row>
            <v-col cols="12">
              <v-text-field label="Name" required v-model="name"></v-text-field>
            </v-col>
            <v-col cols=12>
              <v-label>
                {{$t('explain.learnSwitch')}}
              </v-label>
            </v-col>
            <v-col cols="6" class="text-center">
              <v-btn
                color="green"
                @click="startBind('ON')"
                :outlined="radioOn? false : true"
                dark
              >{{$t('word.bindOn')}}</v-btn>
            </v-col>
            <v-col cols="6" class="text-center">
              <v-btn
                color="red"
                @click="startBind('OFF')"
                :outlined="radioOff? false : true"
                dark
              >{{$t('word.bindOff')}}</v-btn>
            </v-col>
          </v-row>
        </v-container>
      </v-card-text>
      <v-card-actions>
        <div class="flex-grow-1"></div>
        <v-btn color="blue darken-1" text @click="close(false)">{{$t('word.close')}}</v-btn>
        <v-btn
          color="blue darken-1"
          text
          @click="close(true)"
          :disabled="saveOK"
        >{{$t('word.save')}}</v-btn>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>


<script  lang="ts">
import { Component, Vue } from "vue-property-decorator";
import { createDecorator } from "vue-class-component";
import store from "../store";
import "../types";
import { bus } from "../main";

@Component({
  name: "NewSocketMenu"
})
export default class NewSocketMenu extends Vue {
  public dialog: boolean = false;

  name: string = "";
  radioOn: RadioSignal | null = null;
  radioOff: RadioSignal | null = null;

  public show() {
    this.dialog = true;
    this.radioOn = null;
    this.radioOff = null;

    let newName: string = "Socket";

    let counter: number = 0;
    let found: boolean = false;
    this.$store.state.socketConfig.config.forEach((element: SingleConfig) => {
      if (element.name === newName) {
        found = true;
      }
    });

    while (found) {
      counter++;
      newName = "Socket " + String(counter);

      found = false;
      this.$store.state.socketConfig.config.forEach((element: SingleConfig) => {
        if (element.name === newName) {
          found = true;
        }
      });
    }

    this.name = newName;
  }

  private close(save: boolean = false) {
    if (save) {
      let on: RadioSignal = JSON.parse(JSON.stringify(this.radioOn));
      let off: RadioSignal = JSON.parse(JSON.stringify(this.radioOff));

      let newSocket: SingleConfig = {
        name: this.name,
        state: false,
        off: off,
        on: on,
        auto: []
      };
      this.$store.commit("socketConfig/addNewSocket", newSocket);
    }

    this.dialog = false;
  }

  unwatchBind: () => void = () => {};

  private startBind(type: string) {
    this.$store.commit("appState/resetReceivedListen");

    this.unwatchBind();
    if (type === "ON") {
      this.radioOn = null;
    }
    if (type === "OFF") {
      this.radioOff = null;
    }

    this.unwatchBind = bus.$watch(
      "$store.state.appState.receivedRadioConfig",
      (a: RadioSignal, b: any) => {
        if (type === "ON") {
          if (!this.radioOff || this.radioOff.value != a.value) {
            this.radioOn = a;
          } else {
            alert("On and Off must differ!");
          }
        }
        if (type === "OFF") {
          if (!this.radioOn || this.radioOn.value != a.value) {
            this.radioOff = a;
          } else {
            alert("On and Off must differ!");
          }
        }

        this.unwatchBind();
        this.unwatchBind = () => {};
      }
    );
  }

  get saveOK() {

    if(!(this.radioOff && this.radioOn))
    {
      return true;
    }

      let found = false;
      this.$store.state.socketConfig.config.forEach((element: SingleConfig) => {
        if (element.name === this.name) {
          found = true;
        }
      });

    return !(this.radioOff && this.radioOn && !found);
  }

  get onColor() {
    return "green lighten-3";
  }
  get offColor() {
    return "red  lighten-3";
  }
}
</script>


<style lang="scss">
</style>

