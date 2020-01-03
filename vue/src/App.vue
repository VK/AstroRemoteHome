<template>
  <v-app ref="main-window">
    <v-navigation-drawer v-model="drawer" app>
      <v-list>
        <v-list-item @click="openConfigMenu">
          <v-list-item-action>
            <v-icon>mdi-settings</v-icon>
          </v-list-item-action>
          <v-list-item-content>
            <v-list-item-title>{{$t('word.mqttConfig')}}</v-list-item-title>
          </v-list-item-content>
        </v-list-item>

        <v-list-item @click="connect">
          <v-list-item-action>
            <v-icon>mdi-lan-connect</v-icon>
          </v-list-item-action>
          <v-list-item-content>
            <v-list-item-title>{{$t('word.mqttConnect')}}</v-list-item-title>
          </v-list-item-content>
        </v-list-item>

        <v-list-item @click="disconnect">
          <v-list-item-action>
            <v-icon>mdi-lan-disconnect</v-icon>
          </v-list-item-action>
          <v-list-item-content>
            <v-list-item-title>{{$t('word.mqttDisconnect')}}</v-list-item-title>
          </v-list-item-content>
        </v-list-item>

        <v-list-item v-for="(item,i) in $store.state.appState.Esps " :key="i">
          <v-list-item-action style="align-self: start;">
            <v-icon>mdi-chip</v-icon>
          </v-list-item-action>
          <v-list-item-content>
            <v-list-item-title>{{i}}</v-list-item-title>
            <v-list-item-subtitle>
              <v-simple-table>
                <tbody>
                  <tr v-for="(val, key) in item" :key="i+key">
                    <td>{{key}}</td>
                    <td>{{val}}</td>
                  </tr>
                </tbody>
              </v-simple-table>
            </v-list-item-subtitle>
          </v-list-item-content>
        </v-list-item>
      </v-list>
    </v-navigation-drawer>

    <v-app-bar app color="blue darken-2" dark>
      <v-app-bar-nav-icon @click.stop="drawer = !drawer"></v-app-bar-nav-icon>
      <v-toolbar-title>VK AstroRemoteHome</v-toolbar-title>
    </v-app-bar>

    <v-content>
      <v-container class="fill-height" fluid style="min-width:360px; max-width:500px;">
        <v-expansion-panels
          focusable
          dense
          v-if="$store.state.appState.connectionState === 'Connected'"
        >
          <v-expansion-panel
            v-for="(item,i) in $store.state.socketConfig.config "
            :key="i"
            :ref="'expansionPanel'+item.name"
            @click="triggerGoto(item, i)"
            :id="'panel' + i"
          >
            <v-expansion-panel-header>
              <v-switch
                inset
                :label="item.name"
                @click.stop="stopTheEvent"
                @change="triggerManualSwitch($event, item, i)"
                color="orange darken-2"
                v-model="item.state"
                :class=" ['big-input',  item.state ? 'lighton' : 'lightoff']"
              ></v-switch>
            </v-expansion-panel-header>
            <v-expansion-panel-content>
              <v-tabs
                show-arrows
                background-color="grey lighten-4"
                :ref="'switchTab'+item.name"
                style="min-height:384px;"
              >
                <v-tab v-for="(autoSwitch, idx) in item.auto" :key="idx" :arrayIndex="idx">
                  <v-icon>mdi-timelapse</v-icon>
                  &nbsp;{{ idx + 1}}
                </v-tab>
                <v-tab key="10000" @click.stop="addNewSwitch(item)">
                  <v-icon>mdi-plus-circle-outline</v-icon>
                </v-tab>

                <v-tab-item
                  v-for="(autoSwitch, idx) in item.auto"
                  :key="idx"
                  class="tab-content text-center"
                >
                  <v-row class="small-row">
                    <v-col class="pa-0">
                      <v-checkbox
                        v-model="autoSwitch.days"
                        :label="$t('days.mo')"
                        value="Mo"
                        color="primary"
                        @change="saveSocket(item)"
                      ></v-checkbox>
                    </v-col>
                    <v-col class="pa-0">
                      <v-checkbox
                        v-model="autoSwitch.days"
                        :label="$t('days.tu')"
                        value="Tu"
                        color="primary"
                        @change="saveSocket(item)"
                      ></v-checkbox>
                    </v-col>
                    <v-col class="pa-0">
                      <v-checkbox
                        v-model="autoSwitch.days"
                        :label="$t('days.we')"
                        value="We"
                        color="primary"
                        @change="saveSocket(item)"
                      ></v-checkbox>
                    </v-col>
                    <v-col class="pa-0">
                      <v-checkbox
                        v-model="autoSwitch.days"
                        :label="$t('days.th')"
                        value="Th"
                        color="primary"
                        @change="saveSocket(item)"
                      ></v-checkbox>
                    </v-col>
                    <v-col class="pa-0">
                      <v-checkbox
                        v-model="autoSwitch.days"
                        :label="$t('days.fr')"
                        value="Fr"
                        color="primary"
                        @change="saveSocket(item)"
                      ></v-checkbox>
                    </v-col>
                    <v-col class="pa-0">
                      <v-checkbox
                        v-model="autoSwitch.days"
                        :label="$t('days.sa')"
                        value="Sa"
                        color="primary"
                        @change="saveSocket(item)"
                      ></v-checkbox>
                    </v-col>
                    <v-col class="pa-0">
                      <v-checkbox
                        v-model="autoSwitch.days"
                        :label="$t('days.su')"
                        value="Su"
                        color="primary"
                        @change="saveSocket(item)"
                      ></v-checkbox>
                    </v-col>
                  </v-row>

                  <v-row class="small-row start-row">
                    <v-col
                      cols="2"
                      class="pa-0 ma-0 fw-bold"
                      style="position:relative; top:-4px;"
                    >{{$t('swtype.start')}}</v-col>
                    <v-col cols="9" class="pa-0 ma-0" style="position:relative; top:3px;">
                      <v-radio-group
                        v-model="autoSwitch.sT"
                        row
                        class="w-100"
                        @change="saveSocket(item)"
                      >
                        <v-radio
                          off-icon="mdi-clock"
                          on-icon="mdi-clock"
                          :label="$t('swtype.time')"
                          value="t"
                          color="primary"
                          class="flex"
                        ></v-radio>
                        <v-radio
                          off-icon="mdi-weather-sunny"
                          on-icon="mdi-weather-sunny"
                          :label="$t('swtype.sunrise')"
                          value="r"
                          color="primary"
                          class="flex"
                        ></v-radio>
                        <v-radio
                          off-icon="mdi-weather-sunset"
                          on-icon="mdi-weather-sunset"
                          :label="$t('swtype.sunset')"
                          value="s"
                          color="primary"
                        ></v-radio>
                      </v-radio-group>
                    </v-col>
                  </v-row>
                  <v-row class="small-row start-row" style="margin-top:-5; padding-bottom:5px;">
                    <v-col cols="1" class="pa-0 ma-0" style="max-width:12.45%; flex:0 0 12.45%;"></v-col>
                    <v-col
                      cols="4"
                      class="pa-0"
                      style="margin-top:-13px;"
                      v-if="autoSwitch.sT === 't'"
                    >
                      <v-btn color="primary" dark text @click="setStartTime(autoSwitch, item)">
                        <v-icon>mdi-clock</v-icon>
                        {{ String(autoSwitch.start).includes(":") ? autoSwitch.start : $t('word.time')}}
                      </v-btn>
                    </v-col>
                    <v-col cols="4" class="pa-0" v-else style="margin-top:-9px;">
                      <v-select
                        :label="$t('word.difference')"
                        v-model="autoSwitch.start"
                        :items="[
                        {text: '-3 h', value:'-180'},
                        {text: '-2 h', value:'-120'},
                        {text: '-1 h', value:'-60'},
                        {text: '-45 min', value:'-45'},
                        {text: '-30 min', value:'-30'},
                        {text: '-15 min', value:'-15'},
                        {text: '-10 min', value:'-10'},
                        {text: '-5 min', value:'-5'},
                        {text: '0 min', value:'0'}, 
                        {text: '+5 min', value:'5'},
                        {text: '+10 min', value:'10'},
                        {text: '+15 min', value:'15'},
                        {text: '+30 min', value:'30'},
                        {text: '+45 min', value:'45'},
                        {text: '+1 h', value:'60'},
                        {text: '+2 h', value:'120'},
                        {text: '+3 h', value:'180'}]"
                        menu-props="auto"
                        @change="saveSocket(item)"
                      ></v-select>
                    </v-col>

                    <v-col cols="1" class="pa-0" style="margin-top:-15px;">
                      <v-icon>mdi-plus-minus</v-icon>
                    </v-col>
                    <v-col cols="4" class="pa-0" style="margin-top:-9px;">
                      <v-select
                        v-model="autoSwitch.sD"
                        :label="$t('word.variation')"
                        :items="[{text: '0 min', value:0}, 
                        {text: '5 min', value:5},
                        {text: '10 min', value:10},
                        {text: '15 min', value:15},
                        {text: '30 min', value:30},
                        {text: '45 min', value:45},
                        {text: '1 h', value:60},
                        {text: '2 h', value:120},
                        {text: '3 h', value:180}]"
                        @change="saveSocket(item)"
                      ></v-select>
                    </v-col>
                  </v-row>

                  <v-row class="tiny-row">
                    <v-col
                      cols="5"
                      class="pa-0 ma-0 text-left"
                      style="margin-top:-15px !important; margin-left: 10px !important; margin-right: -10px !important;"
                    >{{$t('word.irregularity')}}</v-col>
                    <v-col
                      cols="2"
                      class="pa-0 ma-0"
                      style="padding-right:9px !important; margin-top:-15px !important"
                    >
                      <v-icon>mdi-arrow-down</v-icon>
                    </v-col>
                    <v-col cols="5" class="pa-0 ma-0">
                      <v-switch
                        v-model="autoSwitch.irreg"
                        :label="irregLabel(autoSwitch.irreg)"
                        color="primary"
                        @change="checkIrreg(item, autoSwitch)"
                      ></v-switch>
                    </v-col>
                  </v-row>

                  <v-row class="tiny-row irreg-row" v-if="autoSwitch.irreg">
                    <v-col cols="3" class="text-left mini-label">{{$t('word.ontime')}}:</v-col>
                    <v-col cols="9">
                      <v-range-slider
                        v-model="autoSwitch.onrange"
                        max="120"
                        min="1"
                        step="1"
                        hide-details
                        class="align-center"
                        :id="'onRange'+idx"
                        @change="saveSocket(item)"
                      >
                        <template v-slot:prepend>
                          <v-text-field
                            v-model="autoSwitch.onrange[0]"
                            class="mt-0 pt-0"
                            hide-details
                            single-line
                            type="number"
                            style="width: 45px"
                            :id="'onRangeStart'+idx"
                            @change="saveSocket(item)"
                          ></v-text-field>
                        </template>
                        <template v-slot:append>
                          <v-text-field
                            v-model="autoSwitch.onrange[1]"
                            class="mt-0 pt-0"
                            hide-details
                            single-line
                            type="number"
                            style="width: 45px"
                            :id="'onRangeEnd'+idx"
                            @change="saveSocket(item)"
                          ></v-text-field>
                        </template>
                      </v-range-slider>
                    </v-col>
                  </v-row>

                  <v-row class="small-row irreg-row" v-if="autoSwitch.irreg">
                    <v-col cols="3" class="text-left mini-label">{{$t('word.offtime')}}:</v-col>
                    <v-col cols="9">
                      <v-range-slider
                        v-model="autoSwitch.offrange"
                        max="120"
                        min="1"
                        step="1"
                        hide-details
                        class="align-center"
                        :id="'offRange'+idx"
                        @change="saveSocket(item)"
                      >
                        <template v-slot:prepend>
                          <v-text-field
                            v-model="autoSwitch.offrange[0]"
                            class="mt-0 pt-0"
                            hide-details
                            single-line
                            type="number"
                            style="width: 45px"
                            :id="'offRangeStart'+idx"
                            @change="saveSocket(item)"
                          ></v-text-field>
                        </template>
                        <template v-slot:append>
                          <v-text-field
                            v-model="autoSwitch.offrange[1]"
                            class="mt-0 pt-0"
                            hide-details
                            single-line
                            type="number"
                            style="width: 45px"
                            :id="'offRangeEnd'+idx"
                            @change="saveSocket(item)"
                          ></v-text-field>
                        </template>
                      </v-range-slider>
                    </v-col>
                  </v-row>

                  <v-row class="space-row" v-if="autoSwitch.irreg"></v-row>

                  <v-row class="small-row stop-row">
                    <v-col
                      cols="2"
                      class="pa-0 ma-0 fw-bold"
                      style="position:relative; top:-4px;"
                    >{{$t('swtype.end')}}</v-col>
                    <v-col cols="9" class="pa-0 ma-0" style="position:relative; top:3px;">
                      <v-radio-group v-model="autoSwitch.eT" row class="w-100">
                        <v-radio
                          off-icon="mdi-clock"
                          on-icon="mdi-clock"
                          :label="$t('swtype.time')"
                          value="t"
                          color="primary"
                          class="flex"
                        ></v-radio>
                        <v-radio
                          off-icon="mdi-weather-sunny"
                          on-icon="mdi-weather-sunny"
                          :label="$t('swtype.sunrise')"
                          value="r"
                          color="primary"
                          class="flex"
                        ></v-radio>
                        <v-radio
                          off-icon="mdi-weather-sunset"
                          on-icon="mdi-weather-sunset"
                          :label="$t('swtype.sunset')"
                          value="s"
                          color="primary"
                        ></v-radio>
                      </v-radio-group>
                    </v-col>
                  </v-row>

                  <v-row class="small-row stop-row" style="margin-top:-5; padding-bottom:5px;">
                    <v-col cols="1" class="pa-0 ma-0" style="max-width:12.45%; flex:0 0 12.45%;"></v-col>
                    <v-col
                      cols="4"
                      class="pa-0"
                      style="margin-top:-13px;"
                      v-if="autoSwitch.eT === 't'"
                    >
                      <v-btn color="primary" dark text @click="setEndTime(autoSwitch, item)">
                        <v-icon>mdi-clock</v-icon>
                        {{ String(autoSwitch.end).includes(":") ? autoSwitch.end : $t('word.time')}}
                      </v-btn>
                    </v-col>
                    <v-col cols="4" class="pa-0" v-else style="margin-top:-9px;">
                      <v-select
                        :label="$t('word.difference')"
                        v-model="autoSwitch.end"
                        :items="[
                        {text: '-3 h', value:'-180'},
                        {text: '-2 h', value:'-120'},
                        {text: '-1 h', value:'-60'},
                        {text: '-45 min', value:'-45'},
                        {text: '-30 min', value:'-30'},
                        {text: '-15 min', value:'-15'},
                        {text: '-10 min', value:'-10'},
                        {text: '-5 min', value:'-5'},
                        {text: '0 min', value:'0'}, 
                        {text: '+5 min', value:'5'},
                        {text: '+10 min', value:'10'},
                        {text: '+15 min', value:'15'},
                        {text: '+30 min', value:'30'},
                        {text: '+45 min', value:'45'},
                        {text: '+1 h', value:'60'},
                        {text: '+2 h', value:'120'},
                        {text: '+3 h', value:'180'}]"
                        @change="saveSocket(item)"
                      ></v-select>
                    </v-col>

                    <v-col cols="1" class="pa-0" style="margin-top:-15px;">
                      <v-icon>mdi-plus-minus</v-icon>
                    </v-col>
                    <v-col cols="4" class="pa-0" style="margin-top:-9px;">
                      <v-select
                        :label="$t('word.variation')"
                        v-model="autoSwitch.eD"
                        :items="[{text: '0 min', value:0}, 
                        {text: '5 min', value:5},
                        {text: '10 min', value:10},
                        {text: '15 min', value:15},
                        {text: '30 min', value:30},
                        {text: '45 min', value:45},
                        {text: '1 h', value:60},
                        {text: '2 h', value:120},
                        {text: '3 h', value:180}]"
                        @change="saveSocket(item)"
                      ></v-select>
                    </v-col>
                  </v-row>
                  <v-row>
                    <v-col cols="12" class="text-right">
                      <v-btn
                        color="red"
                        text
                        small
                        @click="deleteSelectedSwitch(item)"
                        :disabled="item.auto.length == 0"
                        :dark="item.auto.length > 0"
                      >
                        <v-icon>mdi-delete-outline</v-icon>
                        {{$t('word.delete')}}&nbsp;
                        <v-icon>mdi-timelapse</v-icon>
                        &nbsp;{{idx + 1}}
                      </v-btn>
                    </v-col>
                  </v-row>
                </v-tab-item>

                <v-tab-item key="10000" class="tab-content">
                  <v-container bg fill-height grid-list-md text-xs-center>
                    <v-layout row wrap align-center>
                      <v-flex
                        class="text-center pa-0"
                        style="position:relative; top:140px !important; "
                      >
                        {{$t('explain.addSwitchA')}}
                        <v-icon>mdi-plus-circle-outline</v-icon>
                        {{$t('explain.addSwitchB')}}
                      </v-flex>
                    </v-layout>
                  </v-container>
                </v-tab-item>
              </v-tabs>

              <div class="text-center socket-foot-content grey lighten-3">
                <v-row>
                  <v-col cols="12">
                    <v-btn color="red" dark text small @click="deleteSocket(item)">
                      <v-icon>mdi-delete-outline</v-icon>
                      {{item.name}} {{$t('word.delete')}}
                    </v-btn>
                  </v-col>
                </v-row>
              </div>
            </v-expansion-panel-content>
          </v-expansion-panel>
        </v-expansion-panels>

        <v-btn
          dark
          fab
          bottom
          right
          fixed
          color="orange darken-2"
          @click="addNewSocket"
          v-if="$store.state.appState.connectionState === 'Connected'"
        >
          <v-icon>mdi-plus</v-icon>
        </v-btn>
        <v-btn dark fab bottom right fixed color="orange darken-2" @click="connect" v-else>
          <v-icon>mdi-lan-connect</v-icon>
        </v-btn>
      </v-container>
      <div class="buffer-item"></div>
    </v-content>
    <v-footer color="blue darken-2" app>
      <span class="white--text mr-auto">2019 Viktor Krückl</span>
      <span class="white--text">{{$t('connState.' + $store.state.appState.connectionState)}}</span>
      <span class="white--text" style="width:70px"></span>
    </v-footer>
    <Config-Menu ref="configMenu" />
    <NewSocketMenu ref="newSocketMenu" />
    <SetTimeMenu ref="setTimeMenu" />
    <DeleteSwitchMenu ref="deleteSwitchMenu" />
    <DeleteSocketMenu ref="deleteSocketMenu" />
  </v-app>
</template>

<script lang="ts">
import { Component, Vue } from "vue-property-decorator";
import { bus } from "./main";
import ConfigMenu from "./components/ConfigMenu.vue";
import NewSocketMenu from "./components/NewSocketMenu.vue";
import SetTimeMenu from "./components/SetTimeMenu.vue";
import DeleteSwitchMenu from "./components/DeleteSwitchMenu.vue";
import DeleteSocketMenu from "./components/DeleteSocketMenu.vue";
import MySocketsConfig from "./store/MySocketsConfig";
import store from "./store";
import goTo from "vuetify/src/services/goto";

@Component({
  name: "App",
  components: {
    ConfigMenu,
    NewSocketMenu,
    SetTimeMenu,
    DeleteSwitchMenu,
    DeleteSocketMenu
  }
})
export default class App extends Vue {
  constructor() {
    super();
  }

  mounted() {
    let that = this;
    setTimeout(function() {
      window.scrollTo(0, 10);
    }, 1000);
  }

  public source: String = "";

  public drawer: any = null;

  private triggerGoto(item: SingleConfig, i: number) {
    setTimeout(function() {
      goTo("#panel" + i, {
        duration: 500,
        offset: 0.74 * window.innerHeight - 400
      });
    }, 300);
  }

  private irregLabel(input: boolean) {
    if (input) {
      return this.$t("word.irregOn");
    } else {
      return this.$t("word.irregOff");
    }
  }

  private openConfigMenu() {
    let configMenu = this.$refs["configMenu"] as ConfigMenu;
    configMenu.show();
  }

  private stopTheEvent() {}

  private triggerManualSwitch(event: Event, item: SingleConfig, i: number) {
    this.$store.commit("socketConfig/toggleSwitch", {
      name: item.name,
      value: event
    });
  }

  private connect() {
    this.$store.commit("appState/connectMQTT");
  }

  private disconnect() {
    this.$store.commit("appState/disconnectMQTT");
  }

  private addNewSocket() {
    let newSocketMenu = this.$refs["newSocketMenu"] as NewSocketMenu;
    newSocketMenu.show();
  }

  private addNewSwitch(singleConfig: SingleConfig) {
    this.$store.commit("socketConfig/addNewSwitch", singleConfig);

    //Hacky send the last added tab as active :)
    this.$nextTick(() => {
      let tabHandle: TabHandle = (this.$refs[
        "switchTab" + singleConfig.name
      ] as unknown[])[0] as TabHandle;
      tabHandle._data.internalLazyValue = singleConfig.auto.length;
    });
  }

  private deleteSelectedSwitch(singleConfig: SingleConfig) {
    let deleteSwitchMenu = this.$refs["deleteSwitchMenu"] as DeleteSwitchMenu;

    let tabHandle: TabHandle = (this.$refs[
      "switchTab" + singleConfig.name
    ] as unknown[])[0] as TabHandle;

    deleteSwitchMenu.startDelete(singleConfig, tabHandle);
  }

  private deleteSocket(singleConfig: SingleConfig) {
    let deleteSocketMenu = this.$refs["deleteSocketMenu"] as DeleteSocketMenu;
    let component = this.$refs["expansionPanel" + singleConfig.name] as Vue[];
    deleteSocketMenu.startDelete(singleConfig, component);
  }

  /**
   * Save the new socket config via mqtt
   */
  private saveSocket(singleConfig: SingleConfig) {
    this.$store.commit("socketConfig/saveSocket", singleConfig);
  }

  /**
   * Set the irregular switching ranges if not set and save the new switchting
   */
  private checkIrreg(
    singleConfig: SingleConfig,
    autoSwitch: AutomaticSwitching
  ) {
    if (autoSwitch.irreg) {
      if (!autoSwitch.onrange) {
        autoSwitch.onrange = [10, 30];
      }
      if (!autoSwitch.offrange) {
        autoSwitch.offrange = [60, 120];
      }
    }

    this.saveSocket(singleConfig);
  }

  private setStartTime(
    autoSwitch: AutomaticSwitching,
    singleConfig: SingleConfig
  ) {
    let setTimeMenu = this.$refs["setTimeMenu"] as SetTimeMenu;

    setTimeMenu.show(
      (time: string) => {
        autoSwitch.start = time;
        this.saveSocket(singleConfig);
      },
      autoSwitch.start,
      "Set Start"
    );
  }

  private setEndTime(
    autoSwitch: AutomaticSwitching,
    singleConfig: SingleConfig
  ) {
    let setTimeMenu = this.$refs["setTimeMenu"] as SetTimeMenu;

    setTimeMenu.show(
      (time: string) => {
        autoSwitch.end = time;
        this.saveSocket(singleConfig);
      },
      autoSwitch.end,
      "Set End"
    );
  }
}
</script>


<style lang="scss">
.tab-content {
  padding-top: 12px;
  margin-top: 0px;
  margin-bottom: 0px;
  margin-left: 28px;
  margin-right: 20px;
  min-height: 340px;
  max-height: 460px;
}

.big-input {
  width: 95%;
  height: 100%;
}

.big-input > div {
  width: 95% !important;
  height: 100% !important;
  margin-top: 10px !important;
}
.big-input label {
  width: 95% !important;
  height: 100% !important;
}

.socket-foot-content {
  height: 48px;
  padding: 10px 24px 10px 24px;
}
.socket-foot-content .col {
  padding: 0px;
}

/*


.v-expansion-content .col {
  padding: 0px;
}
*/

.col .v-input--selection-controls__input {
  margin-right: -1px;
}

.v-expansion-panel-content__wrap {
  padding: 0 !important;
}

.v-expansion-panel-header {
  padding-top: 5px !important;
  padding-bottom: 0 !important;
}

.buffer-item {
  height: 50px;
  width: 100%;
}

.v-expansion-panel {
  background: #f5f5f5e0 !important;
  margin-top: 2px;
  margin-bottom: 2px;
}

.big-input label {
  font-weight: 700;
  font-size: 20px;
}

.big-input.lighton label {
  color: #f57c00;
  text-shadow: 0px 0px 2px rgb(255, 250, 176);
}

.big-input.lightoff label {
  color: rgb(112, 112, 112);
  text-shadow: 0px 0px 2px rgb(255, 255, 255);
}

.v-data-table td,
.v-data-table th {
  padding: 0 !important;
  height: 20px !important;
  color: rgb(150, 150, 150);
}

.small-row {
  height: 50px;
  align-items: center;
}
.tiny-row {
  height: 42px;
  align-items: center;
}
.space-row {
  height: 15px;
}

.fw-bold {
  font-weight: 700;
}
.w-100 {
  width: 100% !important;
}
.w-100 > div {
  width: 100% !important;
}

/*mark delete elementes */
.v-tab.markDelete {
  background-color: rgb(255, 187, 187);
  animation-name: markDeleteColor;
  animation-duration: 2s;
  animation-iteration-count: infinite;
}

.v-expansion-panel.markDelete {
  background-color: rgb(255, 187, 187);
  animation-name: markDeleteColor;
  animation-duration: 2s;
  animation-iteration-count: infinite;
  box-shadow: 2px 2px 2px 1px rgba(255, 0, 0, 0.2);
}

@keyframes markDeleteColor {
  0% {
    background-color: #fff;
  }
  50% {
    background-color: rgb(255, 187, 187);
  }
  100% {
    background-color: #fff;
  }
}

.v-radio .v-label {
  margin-top: 2px;
  margin-left: 2px;
  margin-right: -15px;
  font-size: 70%;
}

.v-item--active > .v-label {
  color: #1976d2 !important;
  text-decoration: underline;
}
.start-row {
  border-left: 5px solid #4caf50;
  margin-left: -28px;
  border-right: 5px solid #4caf50;
  margin-right: -20px;
  background: rgb(236, 253, 237);
}
.stop-row {
  border-left: 5px solid #f44336;
  margin-left: -28px;
  border-right: 5px solid #f44336;
  margin-right: -20px;
  background: rgb(255, 240, 239);
}
.irreg-row {
  border-left: 5px solid #329ce2;
  margin-left: -28px;
  border-right: 5px solid #329ce2;
  margin-right: -20px;
  background: #dfedf7;
}

.v-input--selection-controls {
  margin-top: 0px !important;
  margin-bottom: 0px !important;
  padding-top: 10px !important;
}

.mini-label {
  font-size: 0.75rem !important;
  font-weight: 900;
}

.tab-content .v-input--switch .v-label {
  margin-left: 10px !important;
}
</style>