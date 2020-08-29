<template>
  <v-dialog v-model="dialog" persistent max-width="600px">
    <v-card v-if="locked">
      <v-card-title>
        <span class="headline">{{$t('word.espConfig')}}</span>
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
        <span class="headline">
          {{$t('word.espConfig')}}
          <br />
          {{mac}}
        </span>
      </v-card-title>
      <v-card-text>
        <v-container>
          <v-row dense class="align-center">
            {{$t("explain.addNetworkDevice")}}
          </v-row>
          <v-row dense class="align-center">
            <v-col cols="10" class="ma-0">
              <v-text-field :label="$t('word.newdevice')" required v-model="newIp"></v-text-field>
            </v-col>
            <v-col cols="2" class="ma-0">
              <v-btn color="blue darken-1" text @click="additem">
                <v-icon>mdi-plus-circle</v-icon>
              </v-btn>
            </v-col>
          </v-row>
          <v-row dense>
            <v-col cols="12" class="title font-weight-bold black--text">{{$t("word.knowndevice")}}:</v-col>
          </v-row>

          <v-row dense v-for="(item,i) in devices" :key="i" class="subtitle-1 align-center">
            <v-col cols="7" class="font-weight-black" >{{i}}</v-col>
            <v-col cols="3">{{item.state}}</v-col>
            <v-col cols="2">
              <v-btn color="red darken-1" text @click="deleteitem(i)">
                <v-icon>mdi-minus-circle-outline</v-icon>
              </v-btn>
            </v-col>
          </v-row>
        </v-container>
      </v-card-text>
      <v-card-actions>
        <v-btn color="red darken-1" text @click="reboot">{{$t('word.reboot')}}</v-btn>
        <div class="flex-grow-1"></div>
        <v-btn color="blue darken-1" text @click="close">{{$t('word.close')}}</v-btn>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>


<script  lang="ts">
import { Component, Vue } from "vue-property-decorator";
import { createDecorator } from "vue-class-component";
import store from "../store";

@Component({
  name: "EspConfigMenu"
})
export default class EspConfigMenu extends Vue {
  public dialog: boolean = false;

  mac: string = "";
  newIp: string = "";
  devices: { [ip: string]: SingleDevice } = {};

  public show(inmac: string) {
    this.dialog = true;
    this.locked = true;
    this.pin = "";
    this.devices = this.$store.state.appState.NetworkDevices[inmac];
    console.log(this.devices);

    this.mac = inmac;
    this.newIp = "";
  }

  locked: boolean = true;
  pin: string = "";
  private changePin() {
    if (this.pin && this.pin === this.$store.state.appState.pin) {
      this.locked = false;
    }
  }
  private close() {
    this.dialog = false;
  }
  private additem() {
    store.commit("appState/addNetworkDevice", {
      mac: this.mac,
      ip: this.newIp
    });
    this.devices[this.newIp] = { state: "new" };
    this.$nextTick(() => this.$forceUpdate());
  }

  private deleteitem(name: string) {
    store.commit("appState/removeNetworkDevice", { mac: this.mac, ip: name });
    delete this.devices[name];
    this.$nextTick(() => this.$forceUpdate());
  }


  
  private reboot(name: string) {
    store.commit("appState/rebootDevice", { mac: this.mac});
  }
}
</script>


<style lang="scss">
</style>

