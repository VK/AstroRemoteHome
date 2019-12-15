import Vue from "vue";
import { Store } from "vuex";
import Vuex from "vuex";

import appState from "./store/AppState";
import socketConfig from "./store/MySocketsConfig";

Vue.use(Vuex);

const store: Store<any> = new Store({
  modules: {
    socketConfig,
    appState
  }, strict: false
});


export default store;