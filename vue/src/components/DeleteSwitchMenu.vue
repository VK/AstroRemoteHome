<template>
  <v-dialog v-model="dialog" persistent max-width="600px">
    <v-card>
      <v-card-title>
        <span class="headline">{{$t('word.deleteSwitch')}}</span>
      </v-card-title>
      <v-card-text>
        <v-container>
          <v-row dense>
            <v-col cols="12">
              <v-label>{{$t('explain.deleteSwitch', {name:config.name})}}</v-label>
            </v-col>
          </v-row>
        </v-container>
      </v-card-text>
      <v-card-actions>
        <div class="flex-grow-1"></div>
        <v-btn color="blue darken-1" text @click="close(false)">{{$t('word.close')}}</v-btn>
        <v-btn color="red" text @click="close(true)">{{$t('word.delete')}}</v-btn>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>


<script  lang="ts">
import { Component, Vue } from "vue-property-decorator";
import { createDecorator } from "vue-class-component";
import store from "../store";

@Component({
  name: "DeleteSwitchMenu"
})
export default class DeleteSwitchMenu extends Vue {
  public dialog: boolean = false;

  private config: any = { name: "" };
  private tabHandle?: TabHandle = undefined;

  public startDelete(config: SingleConfig, tabHandle: TabHandle) {
    this.dialog = true;
    this.config = config;
    this.tabHandle = tabHandle;

    //search the tab header, which element is active, then mark it additionally with an animation
    tabHandle.$children[0].$children.forEach((element: Vue) => {
      if (element.$data["isActive"] == true) {
        element.$el.classList.add("markDelete");
      }
    });
  }

  private close(doDelete: boolean = false) {
    let tabHandle = this.tabHandle as TabHandle;
    let config = this.config as SingleConfig;

    //search the tab header, which element is active, and remove the animation
    tabHandle.$children[0].$children.forEach((element: Vue) => {
      if (element.$data["isActive"] == true) {
        element.$el.classList.remove("markDelete");
      }
    });

    if (doDelete) {
      //search the tab header, which element is active, then use the arrayIndex entry to send the right ID to delete in the store
      tabHandle.$children[0].$children.forEach((element: Vue) => {
        if (element.$data["isActive"] == true) {
          if (element.$attrs["arrayIndex"] !== undefined) {
            this.$store.commit("socketConfig/deleteSwitch", {
              singleConfig: config,
              arrayIndex: element.$attrs["arrayIndex"]
            });
          }
        }
      });
    }
    this.dialog = false;
  }
}
</script>


<style lang="scss">
</style>

