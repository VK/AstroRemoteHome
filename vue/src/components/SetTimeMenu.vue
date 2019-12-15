<template>
  <v-dialog v-model="dialog" persistent max-width="300px">
    <v-time-picker format="24hr" v-model="time" @change="setTime" ref="timePicker"></v-time-picker>
  </v-dialog>
</template>


<script  lang="ts">
import { Component, Vue } from "vue-property-decorator";
import { createDecorator } from "vue-class-component";
import store from "../store";

interface TimePicker {
  selecting: number;
}

@Component({
  name: "SetTimeMenu"
})
export default class SetTimeMenu extends Vue {
  public dialog: boolean = false;

  public setTimeCallback: (time: string) => void = (x: string) => {};
  public title: string = "";
  public time: string = "00:00";

  public show(
    setTimeCallback: (time: string) => void,
    inputTime: string,
    title: string = "Set Time"
  ) {
    this.dialog = true;

    if (this.$refs["timePicker"]) {
      let el = (this.$refs["timePicker"] as unknown) as TimePicker;
      el.selecting = 1;
    }

    if (inputTime.split(":").length == 2) {
      this.time = inputTime;
    } else {
      this.time = inputTime;
      this.time = "00:00";
    }
    this.title = title;
    this.setTimeCallback = setTimeCallback;
  }

  private setTime() {
    //set the time
    this.setTimeCallback(this.time);

    this.dialog = false;
  }
}
</script>


<style lang="scss">
</style>

