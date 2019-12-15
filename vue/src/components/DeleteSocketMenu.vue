<template>
  <v-dialog v-model="dialog" persistent max-width="600px">
    <v-card>
      <v-card-title>
        <span class="headline">{{$t('word.deleteSocket')}}</span>
      </v-card-title>
      <v-card-text>
        <v-container>
          <v-row dense>
            <v-col cols="12">
              <v-label>{{$t('explain.deleteSocket', {name:config.name})}}</v-label>
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
import VueComponent from "vue";
import store from "../store";

@Component({
  name: "DeleteSocketMenu"
})
export default class DeleteSocketMenu extends Vue {
  public dialog: boolean = false;

  private config?: any = { name: "" };
  private component?: VueComponent = undefined;

  public startDelete(config: SingleConfig, component: Vue[]) {
    this.dialog = true;
    this.config = config;
    this.component = component[0] as VueComponent;
    this.component.$el.classList.add("markDelete");
  }

  private close(doDelete: boolean = false) {
    let config = this.config as SingleConfig;
    let component = this.component as VueComponent;
    component.$el.classList.remove("markDelete");
    if (doDelete) {
      this.$store.commit("socketConfig/deleteSocket", config);
    }

    this.dialog = false;
  }
}
</script>


<style lang="scss">
</style>

