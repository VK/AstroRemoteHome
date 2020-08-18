<template>
  <v-dialog v-model="dialog" persistent max-width="600px">
    <v-card v-if="locked">
      <v-card-title>
        <span class="headline">{{$t('word.mqttConfig')}}</span>
      </v-card-title>
      <v-card-text>
        <v-container>
          <v-row dense>
            <v-col cols="12">{{$t('explain.editConfig')}}</v-col>
            <v-col cols="12">
              <v-text-field label="Pin" required v-model="pin" type="password" @input="changePin"></v-text-field>
            </v-col>
          </v-row>
        </v-container>
      </v-card-text>
      <v-card-actions>
        <div class="flex-grow-1"></div>
        <v-btn color="blue darken-1" text @click="close(false)">{{$t('word.close')}}</v-btn>
      </v-card-actions>
    </v-card>
    <v-card v-else>
      <v-card-title>
        <span class="headline">{{$t('word.mqttConfig')}}</span>
      </v-card-title>
      <v-card-text>
        <v-container>
          <v-row dense>
            <v-col cols="12">
              <v-text-field label="Server" required v-model="server"></v-text-field>
            </v-col>
            <v-col cols="12">
              <v-text-field label="User" required v-model="user"></v-text-field>
            </v-col>
            <v-col cols="12">
              <v-text-field label="Password" type="password" required v-model="password"></v-text-field>
            </v-col>
            <v-col cols="12">
              <v-text-field label="Port" type="number" required v-model="port"></v-text-field>
            </v-col>
          </v-row>
          <v-row dense justify="center" class="w-100">
            <v-radio-group v-model="useSSL" row class="pa-0 ma-0">
              <v-col cols="6"  class="pa-0 ma-0 flex-center" >
                <v-radio label="no SSL" :value="false"></v-radio>
              </v-col>
              <v-col cols="5"  class="pa-0 ma-0 w-25 flex-center">
                <v-radio label="with SSL" :value="true"></v-radio>
              </v-col>
            </v-radio-group>
          </v-row>
        </v-container>
      </v-card-text>
      <v-card-actions>
        <div class="flex-grow-1"></div>
        <v-btn color="blue darken-1" text @click="close(false)">{{$t('word.close')}}</v-btn>
        <v-btn color="blue darken-1" text @click="close(true)">{{$t('word.save')}}</v-btn>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>


<script  lang="ts">
import { Component, Vue } from "vue-property-decorator";
import { createDecorator } from "vue-class-component";
import store from "../store";

@Component({
  name: "ConfigMenu",
})
export default class ConfigMenu extends Vue {
  public dialog: boolean = false;

  server: string = "";
  user: string = "";
  password: string = "";
  port: number = 0;
  useSSL: boolean = true;

  public show() {
    this.dialog = true;
    this.locked = true;
    this.pin = "";

    this.server = this.$store.state.appState.mqttServer;
    this.user = this.$store.state.appState.mqttUser;
    this.password = this.$store.state.appState.mqttPassword;
    this.port = this.$store.state.appState.mqttPort;
    this.useSSL = this.$store.state.appState.useSSL;

    this.server = this.$store.state.appState.mqttServer;

    //if no configuration is set, no lock is used
    if (this.server == "" && this.user == "" && this.password == "") {
      this.locked = false;
    }
  }

  locked: boolean = true;
  pin: string = "";
  private changePin() {
    if (this.pin && this.pin === this.$store.state.appState.pin) {
      this.locked = false;
    }
  }
  private close(save: boolean = false) {
    if (save) {
      let newConfig = {
        server: this.server,
        user: this.user,
        password: this.password,
        port: Number(this.port),
        useSSL: this.useSSL
      };
      this.$store.commit("appState/enableSaveMQTTconfig");
      this.$store.commit("appState/updateMQTTconfig", newConfig);
    }

    this.dialog = false;
  }
}
</script>


<style lang="scss">
.flex-center {
  display: flex;
  flex-direction: column;
  align-items: center;
}
</style>

